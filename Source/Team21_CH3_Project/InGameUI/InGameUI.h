// InGameUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

// 인게임 화면에 표시되는 메인 HUD UI
// 체력, 탄약, 점수, 라운드 정보, 라운드 전환 메시지를 관리한다.
UCLASS()
class TEAM21_CH3_PROJECT_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// 플레이어 체력 UI 갱신
	// HealthBar와 PlayerHPText를 함께 갱신한다.
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	// 탄약 UI 갱신
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	// 점수 및 라운드 UI 갱신
	void UpdateMatchInfo(int32 PlayerScore, int32 AIScore, int32 Round);

	// 라운드 전환 메시지 표시
	// 예: Round Win / Round Lose
	void ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage);

	// 라운드 전환 메시지 숨김
	void HideRoundTransitionMessage();

	// 라운드 전환 메시지가 현재 표시 중인지 확인
	bool IsRoundTransitionMessageVisible() const;

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

	// 탄약 Text
	// 예: 30 / 30
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	// 조준점 이미지
	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	// 플레이어 점수 Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerScoreText;

	// AI 점수 Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AIScoreText;

	// 현재 라운드 Text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundText;

	// 라운드 전환 메인 메시지 Text
	// 예: Round Win, Round Lose
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionText;

	// 라운드 전환 서브 메시지 Text
	// 예: Get Ready for the Next Round
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionSubText;
};