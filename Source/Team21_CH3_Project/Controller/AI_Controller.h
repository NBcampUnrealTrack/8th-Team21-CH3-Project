// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AI_Controller.generated.h"

class UBlackboardData;
class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class APlayerCharacter;

/**
 * 
 */
UCLASS(Blueprintable)
class TEAM21_CH3_PROJECT_API AAI_Controller : public ADetourCrowdAIController
{
	GENERATED_BODY()
	
	friend class ANonPlayerCharacter;
public:
	AAI_Controller(const FObjectInitializer& ObjectInitializer);
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
	//virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnPossess(APawn* InPawn) override;
	void BeginAI(APawn* InPawn);
	void EndAI();

public:
	static const float PatrolRadius;
	static int32 ShowAIDebug;
	static const FName StartPositionKey;
	static const FName TargetCharacterKey;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(BlueprintReadOnly, Category = "AI")
	float AimPitch;
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, const FAIStimulus Stimulus);
};
