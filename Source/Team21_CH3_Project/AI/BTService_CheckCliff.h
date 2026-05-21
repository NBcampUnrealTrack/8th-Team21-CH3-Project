// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckCliff.generated.h"

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API UBTService_CheckCliff : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckCliff();
protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsCliff;
	UPROPERTY(EditAnywhere, Category = "Settings")
	float CheckDistance = 500.f;
};
