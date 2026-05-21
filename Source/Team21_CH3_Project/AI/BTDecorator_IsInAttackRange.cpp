// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_IsInAttackRange.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PlayerCharacter.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult == true, TEXT("Super::CalculateRawConditionValue() function has returned false."));

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("InValid AIController."));

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	APlayerCharacter* TargetPlayerCharacter = Cast<APlayerCharacter>(BB->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
	if (!TargetPlayerCharacter) return false;
	FVector NPCLocation = NPC->GetActorLocation();
	FVector TargetLocation = TargetPlayerCharacter->GetActorLocation();

	float HorizontalDistance = FVector::Dist2D(NPCLocation, TargetLocation);
	float HeightDifference = FMath::Abs(TargetLocation.Z - NPCLocation.Z);
	float VerticalAttackRange;
	if (NPC->bAttackRange == true)
	{
		VerticalAttackRange = 800.f;
		BB->SetValueAsFloat(AttackRangeKey.SelectedKeyName, 1200.f);
	}
	else
	{
		VerticalAttackRange = 50.f;
		BB->SetValueAsFloat(AttackRangeKey.SelectedKeyName, 45.f);
	}
	float AttackRange = BB->GetValueAsFloat(AttackRangeKey.SelectedKeyName);
	if (IsValid(TargetPlayerCharacter) == true && TargetPlayerCharacter->IsPlayerControlled() == true)
	{
		if (HorizontalDistance <= AttackRange + 50.f && HeightDifference <= VerticalAttackRange)
		{
			FHitResult SightHitResult;
			FCollisionQueryParams SightParams;
			SightParams.AddIgnoredActor(NPC);

			FVector TraceStart = NPCLocation + FVector(0.f, 0.f, 60.f);
			FVector TraceEnd = TargetPlayerCharacter->GetActorLocation();

			bool bHitWall = GetWorld()->LineTraceSingleByChannel(SightHitResult, TraceStart, TraceEnd, ECC_Visibility, SightParams);

			if (bHitWall && SightHitResult.GetActor() != TargetPlayerCharacter)
			{
				return false;
			}
			return true;
		}
	}
	return false;
}
