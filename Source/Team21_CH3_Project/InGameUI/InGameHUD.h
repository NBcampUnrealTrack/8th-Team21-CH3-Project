// InGameHUD.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TimerManager.h"
#include "InGameHUD.generated.h"

class UInGameUI;
class URoundTransitionWidget;
class UOutGameTransitionWidget;

UCLASS()
class TEAM21_CH3_PROJECT_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	void RefreshWaveUI(
		int32 CurrentWave,
		int32 CurrentKillCount,
		int32 TargetKillCount,
		int32 CurrentGold,
		int32 GoldPerKill
	);

	void RefreshHealthUI(float CurrentHealth, float MaxHealth);
	void RefreshAmmoUI(int32 CurrentAmmo, int32 MaxAmmo);

	void ShowRoundTransitionUI(const FText& MainMessage, const FText& SubMessage);
	void HideRoundTransitionUI();
	bool IsRoundTransitionUIShowing() const;

	// HP 위험 피드백 제거
	// 플레이어 사망, Result UI 표시, 라운드 전환 UI 표시 시 호출한다.
	void HideHPDangerFeedback();

	// 웨이브 전환 / 레벨 리로드 직전 화면을 검게 덮는 FadeOut 재생
	void PlayLevelTransitionFadeOut();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInGameUI> InGameUIClass;

	UPROPERTY()
	UInGameUI* InGameUIInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Round Transition", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<URoundTransitionWidget> RoundTransitionWidgetClass;

	UPROPERTY()
	URoundTransitionWidget* RoundTransitionWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Transition", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UOutGameTransitionWidget> InGameStartTransitionWidgetClass;

	UPROPERTY()
	TObjectPtr<UOutGameTransitionWidget> InGameStartTransitionWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Transition", meta = (AllowPrivateAccess = "true"))
	float InGameStartReadyDelay = 1.0f;

	FTimerHandle InGameStartReadyTimerHandle;

	void PlayGameStartReadyTransition();
};