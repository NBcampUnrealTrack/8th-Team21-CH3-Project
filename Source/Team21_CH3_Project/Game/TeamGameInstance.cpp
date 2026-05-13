// TeamGameInstance.cpp
#include "TeamGameInstance.h"

UTeamGameInstance::UTeamGameInstance(){
	selectedWeaponType = EWeaponType::Rifle;
	mouseSensitivity = 1.0f;
	masterVolume = 1.0f;
	playerTotalKillCount = 0;
	playerScore = 0;
	aiScore = 0;
	bIsWin = false;
	bHasMatchResult = false;
}

void UTeamGameInstance::Init(){
	Super::Init();
	
	LoadSaveGame();
}

EWeaponType UTeamGameInstance::GetSelectedWeaponType() const{
	return selectedWeaponType;
}

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

float UTeamGameInstance::GetMouseSensitivity() const{
	return mouseSensitivity;
}

void UTeamGameInstance::SetMouseSensitivity(float value){
	mouseSensitivity = FMath::Clamp(value, 0.5f, 3.0f);
}

float UTeamGameInstance::GetMasterVolume() const{
	return masterVolume;
}

void UTeamGameInstance::SetMasterVolume(float value){
	masterVolume = FMath::Clamp(value, 0.0f, 1.0f);
}

int32 UTeamGameInstance::GetPlayerTotalKillCount() const{
	return playerTotalKillCount;
}

void UTeamGameInstance::SetPlayerTotalKillCount(int32 killCount){
	playerTotalKillCount = FMath::Max(0, killCount);
}

void UTeamGameInstance::AddPlayerKillCount(int32 killCount){
	int32 safeKillCount = FMath::Max(0, killCount);
	playerTotalKillCount += safeKillCount;
}

int32 UTeamGameInstance::GetPlayerScore() const{
	return playerScore;
}

int32 UTeamGameInstance::GetAIScore() const{
	return aiScore;
}

void UTeamGameInstance::AddPlayerScore(int32 score){
	int32 safeScore = FMath::Max(0, score);
	playerScore += safeScore;
}

void UTeamGameInstance::AddAIScore(int32 score){
	int32 safeScore = FMath::Max(0, score);
	aiScore += safeScore;
}

void UTeamGameInstance::ClearScore(){
	AddPlayerKillCount(playerScore);
}

bool UTeamGameInstance::GetIsWin() const{
	return bIsWin;
}

void UTeamGameInstance::SetIsWin(bool bNewIsWin){
	bIsWin = bNewIsWin;
}

bool UTeamGameInstance::GetMatch() const{
	return bHasMatchResult;
}

void UTeamGameInstance::SetMatch(bool bHasMatch){
	bHasMatchResult = bHasMatch;
}
