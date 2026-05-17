// TraitSubSystem.cpp
#include "TraitSubsystem.h"
#include "Game/TeamGameInstance.h"

ETraitCardState UTraitSubsystem::GetTraitCardState(FName traitId, const FTraitData& traitData) const{
	if (traitId.IsNone())
		return ETraitCardState::Invalid;
	
	if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		const int32 CurrentLevel = GI->GetTraitLevel(traitId);
		if (CurrentLevel >= 5)
		{
			UE_LOG(LogTemp, Warning, TEXT("TraitLevel is 5. Level up Failed"));
			return ETraitCardState::MaxLevel; 
		}
		
		const bool bHasRequiredTrait =
			traitData.RequiredTraitId.IsNone() ||
			traitData.RequiredTraitId == FName(TEXT("None")) ||
			GI->GetTraitLevel(traitData.RequiredTraitId) >= 5;
		
		if (bHasRequiredTrait == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("RequiredTraitId >= 5, bHasRequiredTrait == false"));
			return ETraitCardState::Locked;
		}
		
		if ((GI->GetPlayerGold() >= traitData.unlockGoldCost) == false)
		{
			return ETraitCardState::NotEnoughGold; 
		}
	}
	return ETraitCardState::Upgradeable;
}

bool UTraitSubsystem::TryUpgradeTrait(FName traitId, const FTraitData& traitData){
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false) return false;
	
	if (GetTraitCardState(traitId, traitData) != ETraitCardState::Upgradeable) return false;
	if (GI->SpendPlayerGold(traitData.unlockGoldCost) == false) return false;
	
	GI->TraitLevelUp(traitId);
	GI->SaveGameData();
	
	return true;
}

FPlayerTraitBonus UTraitSubsystem::CalculateTotalTraitBonus(UDataTable* traitDataTable) const{
	FPlayerTraitBonus totalBonus;
	
	if (IsValid(traitDataTable) == false) return totalBonus;
	
	const UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (IsValid(GI) == false) return totalBonus;
	
	const TMap<FName, int32>& TraitLevels = GI->GetTraitLevels();
	
	for (const TPair<FName, int32>& Pair : TraitLevels)
	{
		const FName traitId = Pair.Key;
		const int32 Level = Pair.Value;
		
		const FTraitData* traitData = traitDataTable->FindRow<FTraitData>(
			traitId,
			TEXT("CalculateTotalTraitBonus")
		);
		
		if (traitData == nullptr) continue;
		
		totalBonus.weaponDamageBonus += traitData->weaponDamageBonus * Level;
		totalBonus.maxHPBonus += traitData->maxHPBonus * Level;
		totalBonus.moveSpeedBonus += traitData->moveSpeedBonus * Level;
		totalBonus.reloadSpeedBonus += traitData->reloadSpeedBonus * Level;
		totalBonus.goldGainBonus += traitData->goldGainBonus * Level;
	}
	
	return totalBonus;
}


