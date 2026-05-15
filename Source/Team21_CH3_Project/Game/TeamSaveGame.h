// TeamSaveGame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TeamSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API UTeamSaveGame : public USaveGame{
	GENERATED_BODY()
	
public: 
	UPROPERTY()
	float mouseSensitivity = 1.0f;
	UPROPERTY()
	float masterVolume = 100.0f;
	UPROPERTY()
	int32 playerTotalKillCount = 0;
	UPROPERTY()
	int32 playerGold = 0;
	UPROPERTY()
	TMap<FName, int32> traitLevels;
	
	// Trait System save and load 
	//TArray<OutGameTraitMananger> applyTrait; 
};
