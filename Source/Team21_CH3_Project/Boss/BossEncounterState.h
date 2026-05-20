// BossEncounterState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossEncounterState.generated.h"

class UBossAnnounceWidget;
class ACharacter;
class ASpawnManager;
class UStatusComponent;
class ABossMeteorStrikeActor;

UCLASS()
class TEAM21_CH3_PROJECT_API ABossEncounterState : public AActor{
	GENERATED_BODY()

public:
	ABossEncounterState();

	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere, Category = "Boss", meta = (AllowPrivateAccess))
	TObjectPtr<ACharacter> cachedBoss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Boss" , meta = (AllowPrivateAccess))
	float bossMaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (AllowPrivateAccess))
	float phaseTriggerRatio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", meta = (AllowPrivateAccess))
	int32 requiredCoreCount;
	
	UFUNCTION()
	void HandleBossSpawned(ACharacter* SpawnedBoss);
	UFUNCTION()
	void HandleBossCurrentHPChanged(float CurrentHP);
	UFUNCTION()
	void HandlePhaseObjectBreak();
	void PlayAnnounceAnimation();
	void PhaseGimmickStart();
	
		
	UFUNCTION()
	UStatusComponent* GetStatus();
	
	bool bIsPlayPhase;
	
	int32 objectBreakCount;
	int32 phaseStartObjectCount;
	
#pragma region MeteorStrike
	
private:
	UPROPERTY(EditAnywhere, Category = "Meteor", meta = (AllowPrivateAccess))
	TSubclassOf<ABossMeteorStrikeActor> meteorStrikeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor", meta = (AllowPrivateAccess))
	float meteorInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor", meta = (AllowPrivateAccess))
	float meteorSpawnRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess))
	TObjectPtr<UBossAnnounceWidget> announceWidgetInstance;
	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UBossAnnounceWidget> announceWidgetClass;

	void StartMeteorPattern();
	void StopMeteorPattern();
	void SpawnMeteorPattern();
	
	FTimerHandle MeteorTimerHandle;
	UPROPERTY(EditAnywhere, Category = "Meteor|Spawn")
	int32 meteorCountPerWave;
	
#pragma endregion 
	
};
