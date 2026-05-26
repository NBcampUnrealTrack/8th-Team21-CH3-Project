// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "Controller/AI_Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Character/NonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	if (NPC->bIsNowAttacking == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));


	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	checkf(IsValid(Player) == true, TEXT("Player Is imValid"));

	UBlackboardComponent* BB = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());;
	checkf(IsValid(BB) == true, TEXT("BB Is Invalid"));

	if (Player && BB->GetValueAsFloat(AttackRangeKey.SelectedKeyName) >= 500.f)
	{

		NPC->BeginAttack();
	}
	else
	{
		NPC->BeginAttack();
	}
	return EBTNodeResult::InProgress;
}