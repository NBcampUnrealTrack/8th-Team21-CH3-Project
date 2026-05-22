// BossEncounterState.cpp
#include "Boss/BossEncounterState.h"
#include "Boss/BossMeteorStrikeActor.h"
#include "Boss/BossAnnounceWidget.h"
#include "Character/NonPlayerCharacter.h"
#include "Component/StatusComponent.h"
#include "Gimmick/SpawnManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABossEncounterState::ABossEncounterState()
{	
	PrimaryActorTick.bCanEverTick = false;
	
	firstPhaseTriggerRatio = 0.7f;
	secondPhaseTriggerRatio = 0.1f;
	cachedBoss = nullptr;
	requiredCoreCount = 3;
	
	meteorCountPerWave = 3;
	meteorInterval = 5.0f;
	meteorSpawnRadius = 800.0f;
	objectBreakCount = 0;
	phaseStartObjectCount = 3;
	
	currentState = EPhaseState::NonePhase;
}

void ABossEncounterState::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundSpawnManagers;
	UGameplayStatics::GetAllActorsOfClass(this, ASpawnManager::StaticClass(), FoundSpawnManagers);

	for (AActor* FoundActor : FoundSpawnManagers)
	{
		ASpawnManager* SpawnManager = Cast<ASpawnManager>(FoundActor);
		if (!IsValid(SpawnManager))
		{
			continue;
		}

		SpawnManager->OnBossSpawned.RemoveDynamic(this, &ThisClass::HandleBossSpawned);
		SpawnManager->OnBossSpawned.AddDynamic(this, &ThisClass::HandleBossSpawned);
	}
}

void ABossEncounterState::HandleBossSpawned(ACharacter* SpawnedBoss)
{
	cachedBoss = SpawnedBoss;
	if (IsValid(cachedBoss) == false) return;

	GetStatus()->OnCurrentHPChanged.AddUObject(this, &ThisClass::HandleBossCurrentHPChanged);
}

void ABossEncounterState::HandleBossCurrentHPChanged(float CurrentHP){
	if (IsValid(cachedBoss) == false) return;
	
	const float maxHP = GetStatus()->GetMaxHP();
	const float ratio = maxHP > 0.0f ? CurrentHP / maxHP : 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHP: %.3f / MaxHP: %.3f / Ratio: %.6f"), CurrentHP, maxHP, ratio);

	if (ratio <= firstPhaseTriggerRatio && currentState == EPhaseState::NonePhase)
	{
		UE_LOG(LogTemp, Warning, TEXT("Meteor Start"))
		currentState = EPhaseState::FirstPhase;
		
		PlayAnnounceAnimation();
		StartMeteorPattern();
	}
	
	if (ratio <= secondPhaseTriggerRatio && currentState == EPhaseState::FirstPhase)
	{
		UE_LOG(LogTemp, Warning, TEXT("SecondPhase Start"))
		currentState = EPhaseState::SecondPhase;
		
		// PlayAnnounceAnimation(); - Second Phase Start Message
		// enable God Mode() 
		// HealObject->OnObjectBreaked.RemoveDynamic(this, &ThisClass::HandlePhaseObjectBreak);
		// HealObject->OnObjectBreaked.AddDynamic(this, &ThisClass::HandlePhaseObjectBreak);
	}
	
	if (ratio <= KINDA_SMALL_NUMBER)
	{
		currentState = EPhaseState::End;
		StopMeteorPattern();
		return;
	}
}

void ABossEncounterState::HandlePhaseObjectBreak(){
	++objectBreakCount;
	
	if (objectBreakCount >= phaseStartObjectCount)
	{
		PhaseGimmickEnd();
	}
}

void ABossEncounterState::PhaseGimmickEnd(){
	// disabled God Mode
	// 	PlayAnnounceAnimation(); -disabled God Mod Message
}

void ABossEncounterState::PlayAnnounceAnimation(){
	if (IsValid(announceWidgetClass) == true)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		announceWidgetInstance = CreateWidget<UBossAnnounceWidget>(PC, announceWidgetClass);
		if (IsValid(announceWidgetInstance) == true )
		{
			announceWidgetInstance->AddToViewport();
			announceWidgetInstance->PlayAnnounceAnimation();
		}
	}
}

UStatusComponent* ABossEncounterState::GetStatus(){
	ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(cachedBoss);
	
	if (IsValid(Boss) == false) return nullptr;
	
	UStatusComponent* status = Boss->GetStatusComponent();
	if (IsValid(status) == false) return nullptr;
	
	return status;
}

#pragma region MeteorStrike

void ABossEncounterState::StartMeteorPattern(){
	if (GetWorldTimerManager().IsTimerActive(MeteorTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("MeteorTimerHandle is valid"));
		return;
	}
	
	GetWorldTimerManager().SetTimer(
		MeteorTimerHandle,
		this,
		&ThisClass::SpawnMeteorPattern,
		meteorInterval,
		true
	);
}

void ABossEncounterState::StopMeteorPattern(){
	GetWorldTimerManager().ClearTimer(MeteorTimerHandle);
}

void ABossEncounterState::SpawnMeteorPattern(){
	if (IsValid(meteorStrikeClass) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossEncounterState MeteorStrikeClass is Valid"));
		return;
	}
	
	ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (IsValid(playerCharacter) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("BossEncounterState playerCharacter is Valid"));
		return;
	}
		
	for (int32 i = 0; i < meteorCountPerWave; ++i)
	{
		const FVector2D randomCircle = FMath::RandPointInCircle(meteorSpawnRadius);
		FVector spawnLocation = playerCharacter->GetActorLocation();
		spawnLocation.X += randomCircle.X;
		spawnLocation.Y += randomCircle.Y;
	
		FHitResult Hit;
		const FVector traceStart = spawnLocation + FVector(0, 0, 1000.0f);
		const FVector traceEnd = spawnLocation - FVector(0, 0, 3000.0f);
	
		if (GetWorld()->LineTraceSingleByChannel(Hit, traceStart, traceEnd, ECC_Visibility))
		{
			spawnLocation = Hit.ImpactPoint;
		}
	
		GetWorld()->SpawnActor<ABossMeteorStrikeActor>(
			meteorStrikeClass,
			spawnLocation,
			FRotator::ZeroRotator
		);
	}
}


#pragma endregion 