// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AI_Controller.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Character/NonPlayerCharacter.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"


int32 AAI_Controller::ShowAIDebug(0);
const FName AAI_Controller::TargetCharacterKey(TEXT("TargetCharacter"));


FAutoConsoleVariableRef CVarShowAIDebug(
	TEXT("NXProject.ShowAIDebug"),
	AAI_Controller::ShowAIDebug,
	TEXT(""),
	ECVF_Cheat
	);
AAI_Controller::AAI_Controller(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Config_Sight"));

	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2500.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->SetMaxAge(0.f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetDetected);
}

void AAI_Controller::UpdateControlRotation(float DeltaTime,bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, bUpdatePawn);

	if (bUpdatePawn)
	{
		ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(GetPawn());
		if (NPC)
		{
			FRotator CurrentControlRot = GetControlRotation();
			FRotator ActorRot = NPC->GetActorRotation();

			FRotator RelativeRot = CurrentControlRot - ActorRot;
			RelativeRot.Normalize();

			AimPitch = RelativeRot.Pitch;
		}
	}
}

void AAI_Controller::BeginPlay()
{
	Super::BeginPlay();
	APawn* ControlledPawn = GetPawn();
	if (IsValid(ControlledPawn) == true)
	{
		BeginAI(ControlledPawn);
	}
}

void AAI_Controller::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();

	Super::EndPlay(EndPlayReason);
}

void AAI_Controller::BeginAI(APawn* InPawn)//OnPossess와 동일한 코드 만약 오류발생 혹은 아무도 쓰지않으면 삭제할 것
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent) == true)
	{
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent) == true)
		{
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded == true, TEXT("Fail to run behavior Tree."));

			BlackboardComponent->SetValueAsVector(TargetCharacterKey, InPawn->GetActorLocation());
			if (ShowAIDebug == 1)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginAI()")));
			}
		}
	}
}

void AAI_Controller::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent) == true)
	{
		BehaviorTreeComponent->StopTree();

		if (ShowAIDebug == 1)
		{
			UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("EndIA()")));
		}
	}

}

void AAI_Controller::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent) == true)
	{
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent) == true)
		{
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded == true, TEXT("Fail to run behavior Tree."));

			BlackboardComponent->SetValueAsVector(TargetCharacterKey, InPawn->GetActorLocation());
			if (ShowAIDebug == 1)
			{
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginAI()")));
			}
		}
	}
}

void AAI_Controller::OnTargetDetected(AActor* Actor, const FAIStimulus Stimulus)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
	if (Player && Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject(FName("TargetCharacter"), Player);
	}
	else
	{
		GetBlackboardComponent()->ClearValue(FName("TargetCharacter"));
	}
}