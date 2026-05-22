// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_BossCanCharge.h"
#include "Character/NonPlayerCharacter.h"
#include "Controller/AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter.h"

UBTService_BossCanCharge::UBTService_BossCanCharge()
{
	NodeName = TEXT("BossCanCharge");
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_BossCanCharge::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());

	UBlackboardComponent* BB = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());
	checkf(IsValid(BB) == true, TEXT("BB is Invalid"));

	APlayerCharacter* Player = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	checkf(IsValid(BB) == true, TEXT("BB is Invalid"));
	if (!Player)
	{
		return;
	}
	float Distance = FVector::Dist(NPC->GetActorLocation(), Player->GetActorLocation());
	if (NPC->ActorHasTag(TEXT("Boss")) && Distance < MaxChargeDistance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("BossCanCharge"));
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(bCanChargeKey.SelectedKeyName, true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(bCanChargeKey.SelectedKeyName, false);
	}
}