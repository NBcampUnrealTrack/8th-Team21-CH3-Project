// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_CheckCliff.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTService_CheckCliff::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAI_Controller* AIController = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(AIController->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	FVector Start = NPC->GetActorLocation() + (NPC->GetActorForwardVector() * 100.f);
	FVector End = Start + (FVector::DownVector * 500.f);
	FHitResult Hit; //¹Ù´ÚÀÌ ¾øÀ¸¸é ³¶¶³¾îÁö·Î ÆÇ´Ü

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic);

	BB->SetValueAsBool("bIsCliff", !bHit);

	if (!bHit)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(BB->GetValueAsObject("TargetCharacter"));
		if(Player)
		BB->SetValueAsVector("JumpToTarget", Player->GetActorLocation());

	}
}