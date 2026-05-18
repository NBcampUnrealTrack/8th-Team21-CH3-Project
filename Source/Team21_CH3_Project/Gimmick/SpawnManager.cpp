#include "Gimmick/SpawnManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASpawnManager::StartWave(int32 WaveIndex)
{
	if (!WaveDataTable) return;

	FString RowNameString = FString::Printf(TEXT("Wave_%02d"), WaveIndex);
	FName RowName = FName(*RowNameString);

	CurrentWaveData = WaveDataTable->FindRow<FEnemyWaveDataTable>(RowName, TEXT(""));

	if (CurrentWaveData)
	{

		RemainingNormal = CurrentWaveData->NormalCount;
		RemainingRusher = CurrentWaveData->RusherCount;
		RemainingShooter = CurrentWaveData->ShooterCount;
		RemainingBoss = CurrentWaveData->BossCount;

		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnManager::SpawnRoutine, CurrentWaveData->SpawnInterval, true);

		UE_LOG(LogTemp, Warning, TEXT("Wave %d Started!"), WaveIndex);
	}
}

void ASpawnManager::StopWave()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void ASpawnManager::SpawnRoutine()
{

	bool bAllSpawned = (RemainingNormal <= 0 && RemainingRusher <= 0 && RemainingShooter <= 0 && RemainingBoss <= 0);

	if (bAllSpawned)
	{
		StopWave();
		UE_LOG(LogTemp, Warning, TEXT("Wave Spawning Complete!"));
		return;
	}

	if (RemainingBoss > 0)
	{

		ACharacter* SpawnedBoss = SpawnEnemy(BossEnemyClass);
		RemainingBoss--;

		if (SpawnedBoss && OnBossSpawned.IsBound())
		{
			OnBossSpawned.Broadcast(SpawnedBoss);
		}
	}
	else if (RemainingShooter > 0)
	{
		SpawnEnemy(ShooterEnemyClass);
		RemainingShooter--;
	}
	else if (RemainingRusher > 0)
	{
		SpawnEnemy(RusherEnemyClass);
		RemainingRusher--;
	}
	else if (RemainingNormal > 0)
	{
		SpawnEnemy(NormalEnemyClass);
		RemainingNormal--;
	}
}

ACharacter* ASpawnManager::SpawnEnemy(TSubclassOf<ACharacter> EnemyClass)
{
	if (!EnemyClass || SpawnPoints.Num() == 0) return nullptr;

	int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	AActor* SpawnPoint = SpawnPoints[RandomIndex];

	if (SpawnPoint)
	{
		FVector Location = SpawnPoint->GetActorLocation();
		FRotator Rotation = SpawnPoint->GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ACharacter* SpawnedEnemy = GetWorld()->SpawnActor<ACharacter>(EnemyClass, Location, Rotation, SpawnParams);
		return SpawnedEnemy;
	}

	return nullptr;
}