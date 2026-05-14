// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/EnemyWaveDataTable.h" // 작성하신 데이터 구조체 헤더
#include "SpawnManager.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API ASpawnManager : public AActor
{
    GENERATED_BODY()

public:
    ASpawnManager();

protected:
    virtual void BeginPlay() override;

public:
    /** 웨이브 시작 명령 (GameMode에서 호출) */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave(int32 WaveIndex);

    /** 웨이브 중지 및 타이머 초기화 */
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StopWave();

protected:
    /** 일정한 간격마다 몬스터를 스폰하는 핵심 함수 */
    void SpawnRoutine();

    /** 실제로 월드에 액터를 배치하는 헬퍼 함수 */
    void SpawnEnemy(TSubclassOf<ACharacter> EnemyClass);

protected:
    // --- 에디터 설정 데이터 ---

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Data")
    UDataTable* WaveDataTable;

    // 스폰할 몬스터 블루프린트 클래스들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Monsters")
    TSubclassOf<ACharacter> NormalEnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Monsters")
    TSubclassOf<ACharacter> RusherEnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Monsters")
    TSubclassOf<ACharacter> ShooterEnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Monsters")
    TSubclassOf<ACharacter> BossEnemyClass;

    // 스폰 지점들 (TargetPoint 등을 에디터에서 담아줌)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Location")
    TArray<AActor*> SpawnPoints;

private:
    // --- 내부 상태 관리 변수 ---

    FTimerHandle SpawnTimerHandle;

    FEnemyWaveDataTable* CurrentWaveData; // 현재 읽어온 웨이브 행 데이터

    // 현재 웨이브에서 남은 스폰 횟수 카운트
    int32 RemainingNormal;
    int32 RemainingRusher;
    int32 RemainingShooter;
    int32 RemainingBoss;
};