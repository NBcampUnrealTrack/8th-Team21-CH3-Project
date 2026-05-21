// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_DropAndChase.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DropAndChase::UBTTask_DropAndChase()
{
	NodeName = TEXT("Drop And Chase");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_DropAndChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("AIController is invalid"));

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("NPC is invalid"));

	UCharacterMovementComponent* MoveComp = NPC->GetCharacterMovement();
	checkf(IsValid(MoveComp) == true, TEXT("MoveComp is invalid"));

	AIController->StopMovement();
	MoveComp->SetMovementMode(MOVE_Falling);
	FVector ForwardVector = NPC->GetActorForwardVector();
	ForwardVector.Z = 0.f;
	ForwardVector.Normalize();

	MoveComp->Launch(ForwardVector * PushForce);

	return EBTNodeResult::InProgress;
}

void UBTTask_DropAndChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	if (!NPC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	UCharacterMovementComponent* MoveComp = NPC->GetCharacterMovement();
	if (!MoveComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (MoveComp->IsMovingOnGround())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
		BB->SetValueAsBool(TEXT("bIsCliff"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}