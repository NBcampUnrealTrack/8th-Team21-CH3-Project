// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_JumpObstacle.h"
#include "Character/NonPlayerCharacter.h"
#include "Controller/AI_Controller.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Character/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

UBTService_JumpObstacle::UBTService_JumpObstacle()
{
	NodeName = TEXT("JumpObstacle");
	Interval = 0.1f;
}

void UBTService_JumpObstacle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("AIController is inValid"));
	
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("NPC is inValid"));

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	checkf(IsValid(BB) == true, TEXT("Blackboard is inValid"));

	APlayerCharacter* Player = Cast<APlayerCharacter>(BB->GetValueAsObject(TEXT("TargetActor")));
	if (BB->GetValueAsBool(TEXT("bShouldVault"))) return;
	FVector ForwardVector = NPC->GetActorForwardVector();
	FVector StartLocation = NPC->GetActorLocation();
	float TraceDistance = 400.f;

	FHitResult LowHit;
	FVector LowStart = StartLocation + FVector(0, 0, -40.f);
	FVector LowEnd = LowStart + (ForwardVector * TraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(NPC);
	if (Player)
	{
		QueryParams.AddIgnoredActor(Player);
	}
	bool bHitLow = GetWorld()->LineTraceSingleByChannel(LowHit,LowStart,LowEnd, ECC_WorldStatic, QueryParams);

	if (bHitLow && LowHit.GetActor())
	{
		if (LowHit.GetActor()->ActorHasTag(TEXT("Player")) || LowHit.GetActor()->ActorHasTag(TEXT("Monster")))
		{
			BB->SetValueAsBool(TEXT("bShouldVault"), false);
			return;
		}
	}

	if (bHitLow)
	{
		FHitResult HighHit;
		FVector HighStart = StartLocation + FVector(0, 0, 150.f);
		FVector HighEnd = HighStart + (ForwardVector * (TraceDistance + 50.f));
		
		bool bHitHigh = GetWorld()->LineTraceSingleByChannel(HighHit, HighStart, HighEnd, ECC_WorldStatic, QueryParams);
		if (!bHitHigh)
		{
			BB->SetValueAsBool(TEXT("bShouldVault"), true);
			BB->SetValueAsVector(TEXT("JumpToTarget"), LowHit.Location + (ForwardVector * 200.f));
			return;
		}
		else if (FVector::Dist(StartLocation, HighHit.Location) > FVector::Dist(StartLocation, LowHit.Location) + 50.f)
		{
			BB->SetValueAsBool(TEXT("bShouldVault"), true);
		}
	}
	BB->SetValueAsBool(TEXT("bShouldVault"), false);
}