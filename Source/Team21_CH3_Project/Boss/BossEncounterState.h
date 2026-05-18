// BossEncounterState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEncounterState.generated.h"

class ACharacter;
class ASpawnManager;
class UStatusComponent;

UCLASS()
class TEAM21_CH3_PROJECT_API ABossEncounterState : public AActor{
	GENERATED_BODY()

public:
	ABossEncounterState();

	virtual void BeginPlay() override;

private:
	
	UFUNCTION()
	UStatusComponent* GetStatus();
	
	UFUNCTION()
	void HandleBossSpawned(ACharacter* SpawnedBoss);
	UFUNCTION()
	void HandleBossCurrentHPChanged(float CurrentHP);

	UPROPERTY()
	TObjectPtr<ACharacter> cachedBoss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Boss" , meta = (AllowPrivateAccess))
	float bossMaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (AllowPrivateAccess))
	float phaseTriggerRatio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (AllowPrivateAccess))
	int32 requiredCoreCount;
};
