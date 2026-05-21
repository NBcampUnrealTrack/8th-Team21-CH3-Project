// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_CheckCliff.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTService_CheckCliff::UBTService_CheckCliff()
{
	NodeName = TEXT("CheckCliff");
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_CheckCliff::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	FVector NPCLocation = NPC->GetActorLocation();
	FVector GroundLevelLocation = NPCLocation - FVector(0.f, 0.f, 80.f);
	FVector CheckLocation = GroundLevelLocation + (NPC->GetActorForwardVector() * 100.f);

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem)
	{
		return;
	}

	FNavLocation OutNavLocation;
	FVector QueryExtent = FVector(50.f, 50.f, 250.f);

	bool bHasNavMeshAhead = NavSystem->ProjectPointToNavigation(CheckLocation, OutNavLocation, QueryExtent);

	if (bHasNavMeshAhead == false)
	{
		BB->SetValueAsBool(IsCliff.SelectedKeyName, true);
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(BB->GetValueAsObject("TargetCharacter")))
		{
			BB->SetValueAsVector("JumpToTarget", Player->GetActorLocation());
		}
	}
	else
	{
		BB->SetValueAsBool(IsCliff.SelectedKeyName, false);
	}
}