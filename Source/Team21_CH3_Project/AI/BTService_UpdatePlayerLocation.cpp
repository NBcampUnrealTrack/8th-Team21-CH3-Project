// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdatePlayerLocation.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBTService_UpdatePlayerLocation::UBTService_UpdatePlayerLocation()
{
	NodeName = TEXT("UpdatePlayerLocation");
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_UpdatePlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAI_Controller* AIC = Cast<AAI_Controller>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIC) == true, TEXT("AIController Is imValid"));
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		return;
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		Blackboard->SetValueAsObject(AAI_Controller::TargetCharacterKey, Player);
		AIC->SetFocus(Player);
	}
}