// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_BossCharge.h"
#include "Controller/AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "Character/NonPlayerCharacter.h"
#include "Character/PlayerCharacter.h"

UBTTask_BossCharge::UBTTask_BossCharge()
{
	NodeName = TEXT("BossCharge");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_BossCharge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	bHasDealtDamage = false;
	StartLocation = NPC->GetActorLocation();

	ChargeDirection = NPC->GetActorForwardVector();
	ChargeDirection.Z = 0.f;
	ChargeDirection.Normalize();

	OriginalSpeed = NPC->GetCharacterMovement()->MaxWalkSpeed;
	NPC->GetCharacterMovement()->MaxWalkSpeed = ChargeSpeed;
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("Boss Start Charge"));
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_BossCharge::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!NPC || !NPC || !BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	NPC->AddMovementInput(ChargeDirection, 1.0f);

	if (!bHasDealtDamage)
	{
		APlayerCharacter* Target = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
		if (Target)
		{
			float DistToPlayer = FVector::Dist(NPC->GetActorLocation(), Target->GetActorLocation());

			if (DistToPlayer <= 160.f)
			{
				UGameplayStatics::ApplyDamage(Target, ChargeDamage, AIController, NPC, UDamageType::StaticClass());

				bHasDealtDamage = true;
				if (GEngine)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Target has Damage"));
				}
			}
		}
	}
	float MoveDistance = FVector::Dist(StartLocation, NPC->GetActorLocation());

	if (MoveDistance >= TotalChargeDistance)
	{
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("End Charge"));
		}
		NPC->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}