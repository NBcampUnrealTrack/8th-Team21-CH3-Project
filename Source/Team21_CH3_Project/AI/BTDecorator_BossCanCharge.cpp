// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_BossCanCharge.h"
#include "Character/NonPlayerCharacter.h"
#include "Controller/AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter.h"

UBTDecorator_BossCanCharge::UBTDecorator_BossCanCharge()
{
	NodeName = TEXT("BossCanCharge");
	bAllowAbortNone = false;
	bAllowAbortLowerPri = true;
}

bool UBTDecorator_BossCanCharge::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)const
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());

	if (!NPC->ActorHasTag(TEXT("Boss")))
	{
		return false;
	}

	UBlackboardComponent* BB = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());
	checkf(IsValid(BB) == true, TEXT("BB is Invalid"));

	APlayerCharacter* Player = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Player)
	{
		return false;
	}
	float Distance = FVector::Dist(NPC->GetActorLocation(), Player->GetActorLocation());
	if (Distance > MaxChargeDistance)
	{
		return false;
	}
	return true;
}