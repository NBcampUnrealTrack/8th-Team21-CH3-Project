// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_CheckJumpRoute.h"
#include "Controller/AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Character/NonPlayerCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Engine/Engine.h"

UBTService_CheckJumpRoute::UBTService_CheckJumpRoute()
{
	NodeName = "CheckJumpRoute";
	Interval = 0.1f;
	RandomDeviation = 0.02f;
}

void UBTService_CheckJumpRoute::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIController || BB)
	{
		return;
	}
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	APlayerCharacter* Target = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!NPC || Target)
	{
		return;
	}
	bool bIsCliff = BB->GetValueAsBool(IsCliffKey.SelectedKeyName);
	if (!bIsCliff)
	{
		BB->SetValueAsBool(RequiresJumpKey.SelectedKeyName, false);
		return;
	}

	if (BB->GetValueAsBool(RequiresJumpKey.SelectedKeyName))
	{
		return;
	}

	FVector StartLocation = NPC->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	float HorizontalDist = FVector::Dist2D(StartLocation, TargetLocation);
	float HeightDiff = TargetLocation.Z - StartLocation.Z;
	bool bCanJumpToTarget = (HorizontalDist >= 50.f && HorizontalDist <= 1000.f) && (HeightDiff <= 300.f);

	if (bCanJumpToTarget)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
		if (!NavSystem)
		{
			return;
		}
		if (NavSystem)
		{
			FNavLocation ProjectedNavLocation;
			FVector Extent(150.f, 150.f, 400.f);

			bool bFoundNavMash = NavSystem->ProjectPointToNavigation(TargetLocation, ProjectedNavLocation, Extent);

			if (bFoundNavMash)
			{
				/*
				FHitResult HitResult;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(NPC);
				Params.AddIgnoredActor(Target);

				FVector TraceStart = StartLocation + FVector(0, 0, 50.f);
				FVector TraceEnd = ProjectedNavLocation.Location + FVector(0, 0, 50.f);

				bool bHitWall = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

				if (!bHitWall)
				{
					BB->SetValueAsBool(RequiresJumpKey.SelectedKeyName, true);
					BB->SetValueAsVector(JumpTargetLocationKey.SelectedKeyName, ProjectedNavLocation.Location);
					return;
				}
				*/
				BB->SetValueAsBool(RequiresJumpKey.SelectedKeyName, true);
				BB->SetValueAsVector(TEXT("JumpToTarget"), TargetLocation);

				GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, TEXT("[CheckJumpRoute]"));
				return;
			}
		}
	}
	BB->SetValueAsBool(RequiresJumpKey.SelectedKeyName, false);
}