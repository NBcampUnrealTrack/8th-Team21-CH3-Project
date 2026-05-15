// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI_Controller.generated.h"

class UBlackboardData;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API AAI_Controller : public AAIController
{
	GENERATED_BODY()
	
	friend class ANonPlayerCharacter;
public:
	AAI_Controller();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
	void BeginAI(APawn* InPawn);
	void EndAI();

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

public:
	static const float PatrolRadius;
	static int32 ShowAIDebug;
	static const FName StartPatrolPositionKey;
	static const FName EndPatrolPositionKey;
	static const FName TargetCharacterKey;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	class UAIPerceptionComponent* AIPerception;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	class UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	class UAISenseConfig_Damage* DamageSenseConfig;
};
