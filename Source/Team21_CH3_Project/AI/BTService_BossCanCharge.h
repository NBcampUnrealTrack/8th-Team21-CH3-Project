// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_BossCanCharge.generated.h"

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API UBTService_BossCanCharge : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_BossCanCharge();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector bCanChargeKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float MaxChargeDistance = 1500.f;

};
