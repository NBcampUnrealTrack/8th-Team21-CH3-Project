// TeamGameInstance.cpp
#include "TeamGameInstance.h"
#include "TeamSaveGame.h"
#include "Kismet/GameplayStatics.h"

static const FString SaveSlotName = TEXT("PlayerSave");
static constexpr int32 SaveUserIndex = 0;

UTeamGameInstance::UTeamGameInstance(){
	selectedWeaponType = EWeaponType::Rifle;
	mouseSensitivity = 1.0f;
	masterVolume = 100.0f;
	playerTotalKillCount = 0;
	playerGold = 0;
	bIsWin = false;
	bHasMatchResult = false;

	SavedCurrentWave = 1;
	SavedCurrentGold = 0;
	bHasSavedInGameWaveData = false;
}

void UTeamGameInstance::Init(){
	Super::Init();

	LoadGameData();
}

EWeaponType UTeamGameInstance::GetSelectedWeaponType() const{ return selectedWeaponType; }

void UTeamGameInstance::SetSelectedWeaponType(EWeaponType weaponType){
	switch (weaponType)
	{
	case EWeaponType::Rifle:
	case EWeaponType::Shotgun:
	case EWeaponType::Pistol:
		selectedWeaponType = weaponType;
		break;
	default:
		selectedWeaponType = EWeaponType::Rifle;
		break;
	}
}

float UTeamGameInstance::GetMouseSensitivity() const{ return mouseSensitivity; }

void UTeamGameInstance::SetMouseSensitivity(float value){
	mouseSensitivity = FMath::Clamp(value, 0.5f, 3.0f);
	SaveGameData();
}

float UTeamGameInstance::GetMasterVolume() const{ return masterVolume; }

void UTeamGameInstance::SetMasterVolume(float value){
	masterVolume = FMath::Clamp(value, 0.0f, 100.0f);
	SaveGameData();
}

int32 UTeamGameInstance::GetPlayerTotalKillCount() const{ return playerTotalKillCount; }

void UTeamGameInstance::SetPlayerTotalKillCount(int32 killCount){ playerTotalKillCount = FMath::Max(0, killCount); }

void UTeamGameInstance::AddPlayerKillCount(int32 killCount){
	int32 safeKillCount = FMath::Max(0, killCount);
	playerTotalKillCount += safeKillCount;
	SaveGameData();
}

bool UTeamGameInstance::GetIsWin() const{ return bIsWin; }

void UTeamGameInstance::SetIsWin(bool bNewIsWin){ bIsWin = bNewIsWin; }

bool UTeamGameInstance::GetMatch() const{ return bHasMatchResult; }

void UTeamGameInstance::SetMatch(bool bHasMatch){ bHasMatchResult = bHasMatch; }

void UTeamGameInstance::SaveInGameWaveData(int32 InCurrentWave, int32 InCurrentGold){
	SavedCurrentWave = FMath::Max(1, InCurrentWave);
	SavedCurrentGold = FMath::Max(0, InCurrentGold);
	bHasSavedInGameWaveData = true;
}

void UTeamGameInstance::ClearInGameWaveData(){
	playerGold += SavedCurrentGold;

	UE_LOG(LogTemp, Warning, TEXT("SavedCurrentGold : %d \nplayerGold : %d"), SavedCurrentGold, playerGold);

	SavedCurrentWave = 1;
	SavedCurrentGold = 0;
	bHasSavedInGameWaveData = false;
	SaveGameData();
}

bool UTeamGameInstance::HasSavedInGameWaveData() const{ return bHasSavedInGameWaveData; }

int32 UTeamGameInstance::GetSavedCurrentWave() const{ return SavedCurrentWave; }

int32 UTeamGameInstance::GetSavedCurrentGold() const{ return SavedCurrentGold; }

void UTeamGameInstance::AddPlayerGold(int32 gold){ playerGold += gold; }

int32 UTeamGameInstance::GetPlayerGold() const{ return playerGold; }

void UTeamGameInstance::LoadGameData(){
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
	{
		CurrentSaveGame = Cast<UTeamSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));
	}

	if (IsValid(CurrentSaveGame) == false)
	{
		CurrentSaveGame = Cast<UTeamSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UTeamSaveGame::StaticClass()));
	}

	playerTotalKillCount = CurrentSaveGame->playerTotalKillCount;
	mouseSensitivity = CurrentSaveGame->mouseSensitivity;
	masterVolume = CurrentSaveGame->masterVolume;
	playerGold = CurrentSaveGame->playerGold;
	traitLevels = CurrentSaveGame->traitLevels;
}

void UTeamGameInstance::SaveGameData(){
	if (IsValid(CurrentSaveGame) == false)
	{
		CurrentSaveGame = Cast<UTeamSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UTeamSaveGame::StaticClass()));
	}

	CurrentSaveGame->playerTotalKillCount = playerTotalKillCount;
	CurrentSaveGame->mouseSensitivity = mouseSensitivity;
	CurrentSaveGame->masterVolume = masterVolume;
	CurrentSaveGame->playerGold = playerGold;
	CurrentSaveGame->traitLevels = traitLevels;

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, SaveUserIndex);
}

void UTeamGameInstance::StartNewGame(){
	if (IsValid(CurrentSaveGame) == false)
	{
		CurrentSaveGame = Cast<UTeamSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UTeamSaveGame::StaticClass()));
	}

	selectedWeaponType = EWeaponType::Rifle;
	mouseSensitivity = 1.0f;
	masterVolume = 100.0f;
	playerTotalKillCount = 0;
	bIsWin = false;
	bHasMatchResult = false;
	traitLevels.Empty();
	playerGold = 0;
	SavedCurrentWave = 1;
	SavedCurrentGold = 0;
	bHasSavedInGameWaveData = false;

	SaveGameData();
}

#pragma region TraitSystme

const TMap<FName, int32>& UTeamGameInstance::GetTraitLevels() const{
	return traitLevels;
}

bool UTeamGameInstance::CanUpgradeTrait(FName traitId, const FTraitData& traitData) const{
	if (traitId.IsNone())
		return false;

	const int32 CurrentLevel = GetTraitLevel(traitId);
	if (CurrentLevel >= 5)
		return false;

	const bool bHasRequiredTrait =
		traitData.RequiredTraitId.IsNone() ||
		traitData.RequiredTraitId == FName(TEXT("None")) ||
		GetTraitLevel(traitData.RequiredTraitId) >= 5;
	
	if (bHasRequiredTrait == false) return false;

	return playerGold >= traitData.unlockGoldCost;
}

bool UTeamGameInstance::TryUpgradeTrait(FName traitId, const FTraitData& traitData){
	if (CanUpgradeTrait(traitId, traitData) == false) return false;
	if (SpendPlayerGold(traitData.unlockGoldCost) == false) return false;
	
	traitLevels.FindOrAdd(traitId)++; // Level Up traitLevel
	SaveGameData();
	
	return true;
}

FPlayerTraitBonus UTeamGameInstance::GetTotalTraitBonus(UDataTable* TraitDataTable) const{
	
	return 
}

bool UTeamGameInstance::SpendPlayerGold(int32 Cost){
	return true;
}

int32 UTeamGameInstance::GetTraitLevel(FName TraitId) const{ return 0; }

#pragma endregion
