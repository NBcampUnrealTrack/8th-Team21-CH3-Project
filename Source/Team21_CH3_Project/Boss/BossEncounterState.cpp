// BossEncounterState.cpp
#include "Boss/BossEncounterState.h"
#include "Boss/BossMeteorStrikeActor.h"
#include "Boss/BossAnnounceWidget.h"
#include "Gimmick/BossWaveGimmick.h"
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
	
	CreateAnnounceAnimation();
	
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
		currentState = EPhaseState::FirstPhase;
		UE_LOG(LogTemp, Warning, TEXT("Meteor Start"));
		
		if (IsValid(announceWidgetInstance) == true)
		{
			announceWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			announceWidgetInstance->PlayAnnounceAnimation(FText::FromString(TEXT("보스가 메테오를 소환합니다.")));
		}

		StartMeteorPattern();
	}
	const FName currentMapName(*UGameplayStatics::GetCurrentLevelName(this, true));
	
	bool bIsNormal = currentMapName == TEXT("NormalMap") ? true : false;
	
	if (ratio <= secondPhaseTriggerRatio && currentState == EPhaseState::FirstPhase && bIsNormal == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("SecondPhase Start"))
		currentState = EPhaseState::SecondPhase;
		
		announceWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		announceWidgetInstance->PlayAnnounceAnimation(FText::FromString
			(TEXT("보스가 무적 상태입니다.\n회복 장치를 모두 파괴하세요.")));
		ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(cachedBoss);
		//Boss->POW(true); 		// enable God Mode() 
		// PlayAnnounceAnimation(); - Second Phase Start Message

		for (auto& gimmick : bossWaveGimmicks)
		{
			gimmick->ActivateGimmickObject();
			gimmick->OnObjectBreaked.RemoveDynamic(this, &ThisClass::HandlePhaseObjectBreak);
			gimmick->OnObjectBreaked.AddDynamic(this, &ThisClass::HandlePhaseObjectBreak);
		}
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
	
	if (objectBreakCount >= phaseStartObjectCount) PhaseGimmickEnd();
	
}

void ABossEncounterState::PhaseGimmickEnd(){

	ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(cachedBoss);
	// Boss->POW(false); 	// disabled God Mode
	
	announceWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	announceWidgetInstance->PlayAnnounceAnimation(FText::FromString
	(TEXT("회복 장치가 모두 파괴되었습니다.\n보스의 무적 상태가 해제됩니다.")));
}

void ABossEncounterState::CreateAnnounceAnimation(){
	if (IsValid(announceWidgetClass) == true)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		announceWidgetInstance = CreateWidget<UBossAnnounceWidget>(PC, announceWidgetClass);
		if (IsValid(announceWidgetInstance) == true )
		{
			announceWidgetInstance->AddToViewport();
			announceWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
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