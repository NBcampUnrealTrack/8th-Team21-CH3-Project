// InGameHUD.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameUI;
class URoundTransitionWidget;

UCLASS()
class TEAM21_CH3_PROJECT_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	void RefreshMatchUI(int32 PlayerScore, int32 AIScore, int32 Round);
	void RefreshWaveUI(int32 CurrentWave, int32 CurrentKillCount, int32 TargetKillCount, int32 CurrentGold);

	void RefreshHealthUI(float CurrentHealth, float MaxHealth);
	void RefreshAmmoUI(int32 CurrentAmmo, int32 MaxAmmo);

	void ShowRoundTransitionUI(const FText& MainMessage, const FText& SubMessage);
	void HideRoundTransitionUI();
	bool IsRoundTransitionUIShowing() const;

	// HP 위험 피드백 제거
	// 플레이어 사망, Result UI 표시, 라운드 전환 UI 표시 시 호출한다.
	void HideHPDangerFeedback();

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
};