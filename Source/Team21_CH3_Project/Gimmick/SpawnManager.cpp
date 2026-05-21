#include "Gimmick/SpawnManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Character/NonPlayerCharacter.h"
#include "Component/StatusComponent.h"

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

	ACharacter* SpawnedEnemy = nullptr;
	float TargetMaxHP = 100.0f;
	bool bIsBoss = false;

	if (RemainingBoss > 0)
	{
		SpawnedEnemy = SpawnEnemy(BossEnemyClass);
		TargetMaxHP = BossMaxHealth;
		RemainingBoss--;
		bIsBoss = true;
	}
	else if (RemainingShooter > 0)
	{
		SpawnedEnemy = SpawnEnemy(ShooterEnemyClass);
		TargetMaxHP = ShooterMaxHealth;
		RemainingShooter--;
	}
	else if (RemainingRusher > 0)
	{
		SpawnedEnemy = SpawnEnemy(RusherEnemyClass);
		TargetMaxHP = RusherMaxHealth;
		RemainingRusher--;
	}
	else if (RemainingNormal > 0)
	{
		SpawnedEnemy = SpawnEnemy(NormalEnemyClass);
		TargetMaxHP = NormalMaxHealth;
		RemainingNormal--;
	}


	if (IsValid(SpawnedEnemy))
	{
		UStatusComponent* Status = GetStatus(SpawnedEnemy);
		if (IsValid(Status))
		{

			Status->SetMaxHP(TargetMaxHP);
			Status->SetCurrentHP(TargetMaxHP);

			UE_LOG(LogTemp, Log, TEXT("Monster Spawned with HP: %f"), TargetMaxHP);
		}


		if (bIsBoss && OnBossSpawned.IsBound())
		{
			OnBossSpawned.Broadcast(SpawnedEnemy);
		}
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

UStatusComponent* ASpawnManager::GetStatus(ACharacter* temp)
{
	ANonPlayerCharacter* nonTemp = Cast<ANonPlayerCharacter>(temp);

	if(IsValid(nonTemp)==false)	return nullptr;
	
	UStatusComponent* status = nonTemp->GetStatusComponent();
	if (IsValid(status) == false) return nullptr;

	return status;
}
