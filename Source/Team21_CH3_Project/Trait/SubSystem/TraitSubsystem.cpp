// TraitSubSystem.cpp
#include "TraitSubsystem.h"

#include "Game/TeamGameInstance.h"

bool UTraitSubsystem::CanUpgradeTrait(FName traitId, const FTraitData& traitData) const{
	if (traitId.IsNone())
		return false;
	
	if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		/*
		const int32 CurrentLevel = GI->GetTraitLevel(traitId);
		if (CurrentLevel >= 5)
			return false;
		*/
		const bool bHasRequiredTrait =
			traitData.RequiredTraitId.IsNone() ||
			traitData.RequiredTraitId == FName(TEXT("None")) ||
			GI->GetTraitLevel(traitData.RequiredTraitId) >= 5;
		
		if (bHasRequiredTrait == false) return false;
		
		return GI->GetPlayerGold() >= traitData.unlockGoldCost;
	}
}

bool UTraitSubsystem::TryUpgradeTrait(FName traitId, const FTraitData& traitData){
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false) return false;
	
	if (CanUpgradeTrait(traitId, traitData) == false) return false;
	if (GI->SpendPlayerGold(traitData.unlockGoldCost) == false) return false;
	
	GI->TraitLevelUp(traitId);
	
	return true;
}

FPlayerTraitBonus UTraitSubsystem::CalculateTotalTraitBonus(UDataTable* TraitDataTable) const{
	FName traitId, const FTraitData& traitData
	
	
	
	UDataTable* TraitDataTable
	return 
}


