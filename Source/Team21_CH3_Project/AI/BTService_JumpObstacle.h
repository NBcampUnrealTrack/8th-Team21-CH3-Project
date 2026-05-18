// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_JumpObstacle.generated.h"

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API UBTService_JumpObstacle : public UBTService
{
	GENERATED_BODY()


	
public:
	UBTService_JumpObstacle();
protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
