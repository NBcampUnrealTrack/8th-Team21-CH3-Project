// BossEncounterState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEncounterState.generated.h"

class ACharacter;
class ASpawnManager;

UCLASS()
class TEAM21_CH3_PROJECT_API ABossEncounterState : public AActor
{
	GENERATED_BODY()

public:
	ABossEncounterState();
	
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleBossSpawned(ACharacter* SpawnedBoss);

	UPROPERTY()
	TObjectPtr<ACharacter> CurrentBoss;
};
