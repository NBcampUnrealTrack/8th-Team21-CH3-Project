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

	// 1. 데이터 테이블에서 해당 웨이브 행 찾기
	FString RowNameString = FString::Printf(TEXT("Wave_%02d"), WaveIndex);
	FName RowName = FName(*RowNameString);

	CurrentWaveData = WaveDataTable->FindRow<FEnemyWaveDataTable>(RowName, TEXT(""));

	if (CurrentWaveData)
	{
		// 2. 스폰할 마릿수 초기화
		RemainingNormal = CurrentWaveData->NormalCount;
		RemainingRusher = CurrentWaveData->RusherCount;
		RemainingShooter = CurrentWaveData->ShooterCount;
		RemainingBoss = CurrentWaveData->BossCount;

		// 3. 타이머 시작 (SpawnInterval 간격으로 반복)
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
	// 모든 몬스터를 다 스폰했는지 확인
	bool bAllSpawned = (RemainingNormal <= 0 && RemainingRusher <= 0 && RemainingShooter <= 0 && RemainingBoss <= 0);

	if (bAllSpawned)
	{
		StopWave();
		UE_LOG(LogTemp, Warning, TEXT("Wave Spawning Complete!"));
		return;
	}

	// 우선순위에 따라 한 마리씩 스폰 (보스 -> 슈터 -> 러셔 -> 노멀 순)
	if (RemainingBoss > 0)
	{
		SpawnEnemy(BossEnemyClass);
		RemainingBoss--;
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

void ASpawnManager::SpawnEnemy(TSubclassOf<ACharacter> EnemyClass)
{
	if (!EnemyClass || SpawnPoints.Num() == 0) return;

	// 1. 랜덤 스폰 지점 선택
	int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	AActor* SpawnPoint = SpawnPoints[RandomIndex];

	if (SpawnPoint)
	{
		FVector Location = SpawnPoint->GetActorLocation();
		FRotator Rotation = SpawnPoint->GetActorRotation();

		// 2. 실제 스폰
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<ACharacter>(EnemyClass, Location, Rotation, SpawnParams);
	}
}