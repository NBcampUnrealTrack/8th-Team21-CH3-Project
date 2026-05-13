#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

// 인게임 화면에 표시되는 메인 HUD UI
// 체력, 탄약, Wave 정보, 라운드 전환 메시지, HP 위험 피드백을 관리한다.
UCLASS()
class TEAM21_CH3_PROJECT_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// 플레이어 체력 UI 갱신
	// HealthBar, PlayerHPText, HP 위험 피드백을 함께 갱신한다.
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	// 탄약 UI 갱신
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	// 기존 점수 및 라운드 UI 갱신
	// 기존 코드 호환용으로 유지한다.
	void UpdateMatchInfo(int32 PlayerScore, int32 AIScore, int32 Round);

	// Wave / Kill / Gold UI 갱신
	void UpdateWaveInfo(int32 CurrentWave, int32 CurrentKillCount, int32 TargetKillCount, int32 CurrentGold);

	// 라운드 전환 메시지 표시
	// 예: Round Win / Round Lose
	void ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage);

	// 라운드 전환 메시지 숨김
	void HideRoundTransitionMessage();

	// 라운드 전환 메시지가 현재 표시 중인지 확인
	bool IsRoundTransitionMessageVisible() const;

	// HP 위험 피드백 제거
	// 사망, Result UI 표시, 라운드 전환 등에서 강제로 숨길 때 사용한다.
	void HideHPDangerFeedback();

protected:
	virtual void NativeConstruct() override;

protected:
	// 플레이어 HP ProgressBar
	// WBP_InGameUI 안의 ProgressBar 이름이 HealthBar여야 연결된다.
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	// 플레이어 HP 숫자 Text
	// HealthBar 위에 100 / 100 형태로 표시된다.
	// WBP_InGameUI 안의 TextBlock 이름이 PlayerHPText여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayerHPText;

	// HP 30% 이하일 때 화면 가장자리에 표시할 빨간 위험 효과 Image
	// WBP_InGameUI 안의 Image 이름이 HPDangerVignette여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* HPDangerVignette;

	// 탄약 Text
	// 예: 30 / 30
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	// 조준점 이미지
	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	// 기존 플레이어 점수 Text
	// 기존 코드 호환용으로 유지한다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayerScoreText;

	// 기존 AI 점수 Text
	// 기존 코드 호환용으로 유지한다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* AIScoreText;

	// 기존 라운드 Text
	// 기존 코드 호환용으로 유지한다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundText;

	// 현재 Wave Text
	// WBP_InGameUI 안의 TextBlock 이름이 WaveText여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WaveText;

	// 현재 Wave Kill Count Text
	// WBP_InGameUI 안의 TextBlock 이름이 KillText여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* KillText;

	// 현재 Gold Text
	// WBP_InGameUI 안의 TextBlock 이름이 GoldText여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* GoldText;

	// 라운드 전환 메인 메시지 Text
	// 예: Round Win, Round Lose
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionText;

	// 라운드 전환 서브 메시지 Text
	// 예: Get Ready for the Next Round
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionSubText;

private:
	// HP 비율을 검사해서 위험 피드백을 표시하거나 숨긴다.
	void UpdateHPDangerFeedback(float CurrentHealth, float MaxHealth);
};