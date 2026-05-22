// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BossCharge.h"
#include "Controller/AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "Character/NonPlayerCharacter.h"
#include "Character/PlayerCharacter.h"
#include "TimerManager.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_BossCharge::UBTTask_BossCharge()
{
	NodeName = TEXT("BossCharge");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_BossCharge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	if (UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent())
	{
		PathFollowingComp->AbortMove(*this, FPathFollowingResultFlags::MovementStop);
	}


	AIController->StopMovement();

	if (NPC->GetCharacterMovement())
	{
		NPC->GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
		NPC->GetCharacterMovement()->GroundFriction = 0.f;
		NPC->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	}


	NPC->bIsCharging = true;

	FVector ChargeDirection = NPC->GetActorForwardVector();
	FVector LaunchVelocity = ChargeDirection * ChargeSpeed;
	NPC->LaunchCharacter(LaunchVelocity, true, false);
	
	FTimerHandle ChargeTimerHandle;
	UWorld* World = NPC->GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(ChargeTimerHandle, FTimerDelegate::CreateLambda([this, &OwnerComp, NPC]()
			{
				if (NPC)
				{
					NPC->bIsCharging = false;
					if (NPC->CoolTime.IsBound())
					{
						NPC->CoolTime.Execute();
					}
					
					
					NPC->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_NavWalking);
					NPC->GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
					NPC->GetCharacterMovement()->GroundFriction = 8.f;


					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("EndCharge"));
				}
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}), ChargeDuration, false);

	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("BossCharge"));
	}
	return EBTNodeResult::InProgress;
}