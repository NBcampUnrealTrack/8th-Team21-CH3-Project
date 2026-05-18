// BossEncounterState.cpp
#include "Boss/BossEncounterState.h"
#include "Gimmick/SpawnManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABossEncounterState::ABossEncounterState()
{	TArray<AActor*> FoundSpawnManagers;
	UGameplayStatics::GetAllActorsOfClass(this, ASpawnManager::StaticClass(), FoundSpawnManagers);

	for (AActor* FoundActor : FoundSpawnManagers)
	{
		ASpawnManager* SpawnManager = Cast<ASpawnManager>(FoundActor);
		if (!IsValid(SpawnManager))
		{
			continue;
		}

		//SpawnManager->OnBossSpawned.RemoveDynamic(this, &ThisClass::HandleBossSpawned);
		//SpawnManager->OnBossSpawned.AddDynamic(this, &ThisClass::HandleBossSpawned);
	}
	PrimaryActorTick.bCanEverTick = false;
}

void ABossEncounterState::BeginPlay()
{
	Super::BeginPlay();
}

void ABossEncounterState::HandleBossSpawned(ACharacter* SpawnedBoss)
{
	CurrentBoss = SpawnedBoss;
}
