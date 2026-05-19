// ShooterInGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "Data/AugmentationDataTable.h"
#include "ShooterInGameMode.generated.h"

class UAugmentCardSelectWidget;
class UDataTable;
class UStatusComponent;
class ASpawnManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKilledSignature, AActor*, KilledEnemy);

UCLASS()
class TEAM21_CH3_PROJECT_API AShooterInGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShooterInGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void OnCharacterDied(bool bIsPlayer);

	void StartWave();
	void HandleEnemyDied();
	void ClearWave();

	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void StartNextWave();

	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void ContinueToNextWaveWithLevelReload();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RequestHUDWaveInfoRefreshRetry();

	void EndMatch(bool bPlayerWon);

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyKilledSignature OnEnemyKilledDelegate;

	UFUNCTION(BlueprintCallable, Category = "Events")
	void EnemyKilled(AActor* KilledEnemy);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Data")
	UDataTable* WaveDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	int32 CurrentWave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	int32 CurrentWaveKillCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	int32 TargetKillCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	int32 MaxWave;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	int32 BaseTargetKillCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	int32 TargetKillCountIncreasePerWave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 CurrentGold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reward")
	int32 GoldPerEnemyKill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	bool bIsWaveInProgress;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave State")
	bool bIsShopOpen;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave State")
	float NextWaveStartDelay;

	FTimerHandle NextWaveStartTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	bool bIsMatchEnded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	FName OutGameLevelName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match State")
	float EndMatchReturnDelay;

	FTimerHandle EndMatchReturnTimerHandle;

	FTimerHandle HUDWaveRefreshRetryTimerHandle;

	int32 HUDWaveRefreshRetryCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	int32 MaxHUDWaveRefreshRetryCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	float HUDWaveRefreshRetryInterval;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Augment")
	TSubclassOf<UAugmentCardSelectWidget> AugmentCardSelectWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Augment")
	int32 AugmentKillInterval;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Augment")
	bool bIsAugmentSelectOpen;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Augment")
	bool bPendingClearWaveAfterAugment;

protected:
	int32 CalculateTargetKillCountForWave(int32 InWave) const;
	int32 CalculateGoldPerKillForWave(int32 InWave) const;
	FName MakeWaveDataRowName(int32 InWave) const;

	bool TryStartWaveWithSpawnManager(int32 InWave);

	void AddGold(int32 GoldAmount);

	void RefreshHUDWaveInfo();
	bool TryRefreshHUDWaveInfo();

	void HandleHUDWaveInfoRefreshRetry();

	void ReloadCurrentLevel();
	void MoveToOutGameMap();
	void StopGameplayInput();
	void ResumeGameplayInput();

	void HandleAutoStartNextWaveWithLevelReload();
	void HandleEndMatchReturnToOutGame();

	void ShowAugmentCardSelectUI();
	void HideAugmentCardSelectUI();

	void SavePlayerHPToGameInstance();
	void RestorePlayerHPFromGameInstance();
	UStatusComponent* GetPlayerStatusComponent() const;

	

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void RequestSpawnWave(int32 InWave, int32 InTargetKillCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
	void RequestOpenShop(int32 ClearedWave, int32 CurrentGoldAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void TriggerRoundResultUI(int32 ClearedWave, int32 CurrentGoldAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void TriggerResultUI(bool bPlayerWon);

	// 게임 시작 Transition이 끝난 뒤 Wave를 시작하기 위한 타이머
	FTimerHandle GameStartWaveTimerHandle;

	// 게임 시작 Transition 재생 시간.
	// WBP_InGameUI의 GameStartAnim 길이와 맞춰야 한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave|Transition")
	float GameStartTransitionDelay = 2.2f;

	// Wave Clear 순간 슬로우 모션을 복구하기 위한 타이머
	FTimerHandle WaveClearSlowMotionTimerHandle;

	// Wave Clear 시 적용할 글로벌 슬로우 배율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave|Transition")
	float WaveClearSlowMotionDilation = 0.25f;

	// Wave Clear 슬로우 모션 유지 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave|Transition")
	float WaveClearSlowMotionDuration = 0.35f;

	// Wave Clear 슬로우 모션 시작
	void StartWaveClearSlowMotion();

	// Wave Clear 슬로우 모션 복구
	void RestoreWaveClearSlowMotion();

public:
	UFUNCTION(Exec)
	void CmdKillEnemy();

	UFUNCTION(Exec)
	void CmdClearWave();

	UFUNCTION(Exec)
	void CmdStartNextWave();

	UFUNCTION(Exec)
	void CmdMoveOutGame();

	void NotifyAugmentSelectionComplete();
};