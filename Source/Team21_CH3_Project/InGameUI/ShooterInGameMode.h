// ShooterInGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "ShooterInGameMode.generated.h"

class UAugmentCardSelectWidget;
class UDataTable;
struct FAugmentCardData;

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

	UPROPERTY()
	UAugmentCardSelectWidget* ActiveAugmentCardSelectWidget;

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

	UFUNCTION()
	void HandleAugmentSelected(FAugmentCardData SelectedCardData);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void RequestSpawnWave(int32 InWave, int32 InTargetKillCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
	void RequestOpenShop(int32 ClearedWave, int32 CurrentGoldAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void TriggerRoundResultUI(int32 ClearedWave, int32 CurrentGoldAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void TriggerResultUI(bool bPlayerWon);

public:
	UFUNCTION(Exec)
	void CmdKillEnemy();

	UFUNCTION(Exec)
	void CmdClearWave();

	UFUNCTION(Exec)
	void CmdStartNextWave();

	UFUNCTION(Exec)
	void CmdMoveOutGame();
};