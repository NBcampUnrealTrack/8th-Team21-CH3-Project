// BossEncounterState.cpp
#include "Boss/BossEncounterState.h"
#include "Character/NonPlayerCharacter.h"
#include "Component/StatusComponent.h"
#include "Gimmick/SpawnManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABossEncounterState::ABossEncounterState()
{	
	PrimaryActorTick.bCanEverTick = false;
	
	phaseTriggerRatio = 0.7f;
	bossMaxHP = 1000.0f;
	cachedBoss = nullptr;
	requiredCoreCount = 3;
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
	
	GetStatus()->SetMaxHP(bossMaxHP);
	GetStatus()->SetCurrentHP(bossMaxHP);
	
	UE_LOG(LogTemp, Warning, TEXT("Boss HP Init: Max %.1f / Current %.1f"),
	GetStatus()->GetMaxHP(),
	GetStatus()->GetCurrentHP()
	);

	GetStatus()->OnCurrentHPChanged.AddUObject(this, &ThisClass::HandleBossCurrentHPChanged);
}

UStatusComponent* ABossEncounterState::GetStatus(){
	ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(cachedBoss);
	
	if (IsValid(Boss) == false) return nullptr;
	
	UStatusComponent* status = Boss->GetStatusComponent();
	if (IsValid(status) == false) return nullptr;
	
	return status;
}

void ABossEncounterState::HandleBossCurrentHPChanged(float CurrentHP){
	if (IsValid(cachedBoss) == false) return;
	
	const float maxHP = GetStatus()->GetMaxHP();
	const float ratio = maxHP > 0.0f ? CurrentHP / maxHP : 0.0f;
	
	if (ratio <= phaseTriggerRatio)
	{
		// Boss GimmickStart
	}
}
