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
	int32 mouseSensitivity;
	UPROPERTY()
	float masterVolume;
	UPROPERTY()
	int32 playerTotalKillCount;
	UPROPERTY()
	int32 playerGold;
	
	// Trait System save and load 
	//TArray<OutGameTraitMananger> applyTrait; 
};
