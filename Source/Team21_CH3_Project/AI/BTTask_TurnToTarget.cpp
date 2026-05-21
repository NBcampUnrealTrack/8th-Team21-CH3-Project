// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTargetActor");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("InValid AIController"));

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("InValid NPC."));

	if (ACharacterBase* Target = Cast<ACharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetCharacterKey)))
	{
		FVector LookVector = Target->GetActorLocation() - NPC->GetActorLocation();
		FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
		AIController->SetControlRotation(FMath::RInterpTo(NPC->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 5.f));

		FRotator BodyRotation = FRotator(0.f, TargetRotation.Yaw, 0.f);
		NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(), BodyRotation, GetWorld()->GetDeltaSeconds(), 5.f));

		return Result = EBTNodeResult::Succeeded;
	}
	return Result = EBTNodeResult::Failed;
}