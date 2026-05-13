// ShooterInGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterInGameMode.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API AShooterInGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterInGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// 캐릭터 사망 알림
	// bIsPlayer == true  : 플레이어 사망
	// bIsPlayer == false : Enemy 사망
	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void OnCharacterDied(bool bIsPlayer);

	// 현재 Wave를 시작한다.
	void StartWave();

	// Enemy 사망 처리
	void HandleEnemyDied();

	// 현재 Wave 클리어 처리
	void ClearWave();

	// 다음 Wave 시작
	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void StartNextWave();

	// 게임 종료 처리
	void EndMatch(bool bPlayerWon);

protected:
	// 현재 Wave 번호
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	int32 CurrentWave;

	// 현재 Wave에서 처치한 Enemy 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	int32 CurrentWaveKillCount;

	// 현재 Wave에서 처치해야 하는 Enemy 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	int32 TargetKillCount;

	// 최대 Wave 수
	// 이 Wave를 클리어하면 Result 승리 처리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	int32 MaxWave;

	// 기본 Wave 목표 처치 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	int32 BaseTargetKillCount;

	// Wave마다 증가할 목표 처치 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	int32 TargetKillCountIncreasePerWave;

	// 현재 보유 Gold
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 CurrentGold;

	// Enemy 처치 시 획득 Gold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reward")
	int32 GoldPerEnemyKill;

	// Wave 진행 중 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	bool bIsWaveInProgress;

	// Shop 열림 상태 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	bool bIsShopOpen;

	// Match 종료 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	bool bIsMatchEnded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	FName OutGameLevelName;

protected:
	// 현재 Wave 기준 목표 처치 수 계산
	int32 CalculateTargetKillCountForWave(int32 InWave) const;

	// Gold 추가
	void AddGold(int32 GoldAmount);

	// HUD Wave / Kill / Gold 정보 갱신
	void RefreshHUDWaveInfo();

	// OutGameMap 이동
	void MoveToOutGameMap();

	// 플레이 입력 정지
	void StopGameplayInput();

protected:
	// EnemySpawner에게 Wave 시작을 요청한다.
	// 실제 EnemySpawner 연동 전까지 BP에서 받을 수 있는 이벤트 자리
	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void RequestSpawnWave(int32 InWave, int32 InTargetKillCount);

	// Shop UI Open 요청
	// Wave Clear 후 호출된다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
	void RequestOpenShop(int32 ClearedWave, int32 CurrentGoldAmount);

	// Result UI 호출용
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void TriggerResultUI(bool bPlayerWon);

public:
	// 테스트 명령어: Enemy 1마리 사망 처리
	UFUNCTION(Exec)
	void CmdKillEnemy();

	// 테스트 명령어: 현재 Wave 강제 클리어
	UFUNCTION(Exec)
	void CmdClearWave();

	// 테스트 명령어: 다음 Wave 시작
	UFUNCTION(Exec)
	void CmdStartNextWave();

	// 테스트 명령어: OutGameMap 이동
	UFUNCTION(Exec)
	void CmdMoveOutGame();
};