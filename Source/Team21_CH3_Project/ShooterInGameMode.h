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
	UFUNCTION(BlueprintCallable, Category = "Match Rules")
	void OnCharacterDied(bool bIsPlayer);

	void StartRound();
	void EndRound(bool bPlayerWonRound);
	void EndMatch(bool bPlayerWon);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	int32 CurrentRound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	int32 PlayerScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	int32 AIScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	bool bIsRoundTransitioning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Match State")
	bool bIsMatchEnded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match State")
	float RoundTransitionDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match State")
	int32 TargetScoreToWin;

	FTimerHandle RoundTransitionTimerHandle;

	void FinishRoundTransition();
	void RefreshHUDMatchInfo();
	void ShowRoundTransitionMessage(bool bPlayerWonRound);
	void RestartCurrentLevel();
	void StopGameplayInput();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void TriggerResultUI(bool bPlayerWon);

public:
	UFUNCTION(Exec)
	void CmdAddPlayerScore();

	UFUNCTION(Exec)
	void CmdAddAIScore();
};