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
	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void OnCharacterDied(bool bIsPlayer);

	void StartWave();
	void HandleEnemyDied();
	void ClearWave();

	UFUNCTION(BlueprintCallable, Category = "Wave Rules")
	void StartNextWave();

	void EndMatch(bool bPlayerWon);

protected:
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	bool bIsMatchEnded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	FName OutGameLevelName;

protected:
	int32 CalculateTargetKillCountForWave(int32 InWave) const;
	void AddGold(int32 GoldAmount);
	void RefreshHUDWaveInfo();
	void MoveToOutGameMap();
	void StopGameplayInput();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void RequestSpawnWave(int32 InWave, int32 InTargetKillCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
	void RequestOpenShop(int32 ClearedWave, int32 CurrentGoldAmount);

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