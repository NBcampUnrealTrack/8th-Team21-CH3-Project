// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossCharge.generated.h"

/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API UBTTask_BossCharge : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_BossCharge();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	FVector ChargeDirection;
	FVector StartLocation;
	float OriginalSpeed;

	bool bHasDealtDamage;

public:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetCharacterKey;
	UPROPERTY(EditAnywhere, Category = "ChargeSettings")
	float TotalChargeDistance = 1000.f;
	UPROPERTY(EditAnywhere, Category = "ChargeSettings")
	float ChargeSpeed = 4000.f;
	UPROPERTY(EditAnywhere, Category = "ChargeSettings")
	float ChargeDamage = 80.f;
};
