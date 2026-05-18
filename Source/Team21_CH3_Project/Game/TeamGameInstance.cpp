// TeamGameInstance.cpp
#include "TeamGameInstance.h"
#include "TeamSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Trait/SubSystem/TraitSubsystem.h"

static const FString SaveSlotName = TEXT("PlayerSave");
static constexpr int32 SaveUserIndex = 0;

UTeamGameInstance::UTeamGameInstance(){
	ResetToDefaultValues();
}

void UTeamGameInstance::Init(){
	Super::Init();

	LoadGameData();
	
	ApplyGoldTraitBonus();
}

void UTeamGameInstance::ResetToDefaultValues()
{
	selectedWeaponType = EWeaponType::Rifle;
	mouseSensitivity = 1.0f;
	masterVolume = 100.0f;
	playerTotalKillCount = 0;
	playerGold = 10000;
	goldGainMultiplier = 1.0f;
	bIsWin = false;
	bHasMatchResult = false;
	traitLevels.Empty();
	SavedCurrentWave = 1;
	SavedCurrentGold = 0;
	bHasSavedInGameWaveData = false;
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
	AddPlayerGold(SavedCurrentGold);

	UE_LOG(LogTemp, Warning, TEXT("SavedCurrentGold : %d \nplayerGold : %d"), SavedCurrentGold, playerGold);

	SavedCurrentWave = 1;
	SavedCurrentGold = 0;
	bHasSavedInGameWaveData = false;
	SaveGameData();
}

bool UTeamGameInstance::HasSavedInGameWaveData() const{ return bHasSavedInGameWaveData; }

int32 UTeamGameInstance::GetSavedCurrentWave() const{ return SavedCurrentWave; }

int32 UTeamGameInstance::GetSavedCurrentGold() const{ return SavedCurrentGold; }

void UTeamGameInstance::AddPlayerGold(int32 gold){
	const int32 finalGold = FMath::RoundToInt(gold * goldGainMultiplier);
	playerGold += finalGold;
}

void UTeamGameInstance::SetGoldGainMultiplier(float goldGain){
	goldGainMultiplier = 1.0f + FMath::Max(0, goldGain); 
}

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

	ResetToDefaultValues();

	SaveGameData();
}

#pragma region TraitSystme

void UTeamGameInstance::ApplyGoldTraitBonus(){
	if (traitDataTable == nullptr) return;
	
	UTraitSubsystem* subsystem = GetSubsystem<UTraitSubsystem>();
	if (IsValid(subsystem) == false) return;
	
	const FPlayerTraitBonus bonus = subsystem->CalculateTotalTraitBonus(traitDataTable);

	SetGoldGainMultiplier(bonus.goldGainBonus);
}

const TMap<FName, int32>& UTeamGameInstance::GetTraitLevels() const{
	return traitLevels;
}

bool UTeamGameInstance::SpendPlayerGold(int32 Cost){
	if (playerGold < Cost)
	{
		UE_LOG(LogTemp, Warning, TEXT("playerGold %d Cost %d SpendPlayerGold Failed"), playerGold, Cost);
		return false;
	}
	playerGold -= Cost;
	return true;
}

void UTeamGameInstance::TraitLevelUp(FName traitId){
	traitLevels.FindOrAdd(traitId)++;
	ApplyGoldTraitBonus();
	SaveGameData();
}

int32 UTeamGameInstance::GetTraitLevel(FName traitId) const{
	if (const int32* foundLevel = traitLevels.Find(traitId))
	{
		return *foundLevel;
	}

	return 0;
}

#pragma endregion
