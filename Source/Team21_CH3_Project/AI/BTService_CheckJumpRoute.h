// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckJumpRoute.generated.h"

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API UBTService_CheckJumpRoute : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckJumpRoute();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsCliffKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RequiresJumpKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector JumpTargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxJumpDistance = 3000.f;
};
