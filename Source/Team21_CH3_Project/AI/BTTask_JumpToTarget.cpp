// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_JumpToTarget.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_JumpToTarget::UBTTask_JumpToTarget()
{
	NodeName = TEXT("JumpToTarget");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_JumpToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	AIController->StopMovement();
	FVector TargetLocation = BB->GetValueAsVector("JumpToTarget");
	FVector CurrentLocation = NPC->GetActorLocation();
	FVector LaunchVelocity;

	if (BB->GetValueAsBool(TEXT("bShouldVault")))
	{
		LaunchVelocity = (NPC->GetActorForwardVector() * 450.f) + FVector(0, 0, 900.f);
	}
	else
	{
		FVector Direction = (TargetLocation - CurrentLocation);
		Direction.Z = 0;
		LaunchVelocity = Direction.GetSafeNormal() * 500.f + FVector(0, 0, 500.f);
	}
	NPC->LaunchCharacter(LaunchVelocity, true, true);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_JumpToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	UCharacterMovementComponent* MoveComp = NPC->GetCharacterMovement();

	bool bIsLanded = (MoveComp->IsFalling() == false) || (MoveComp->IsMovingOnGround()) || (NPC->GetVelocity().IsNearlyZero(1.f));
	if (bIsLanded)
	{
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsBool(TEXT("bShouldVault"), false);
			BB->SetValueAsBool(TEXT("bIsCliff"), false);
			BB->SetValueAsBool(TEXT("JumpNPC"), false);

			BB->ClearValue(TEXT("JumpToTarget"));

		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	}
}