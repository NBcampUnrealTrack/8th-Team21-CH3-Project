// InGameUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Widget.h"
#include "InGameUI.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UWidget;
class UTexture2D;
class UWidgetAnimation;


// 인게임 화면에 표시되는 메인 HUD UI
// 체력, 탄약, Wave, Kill, Gold 정보, 라운드 전환 메시지, HP 위험 피드백을 관리한다.
UCLASS()
class TEAM21_CH3_PROJECT_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// 플레이어 체력 UI 갱신
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	// 탄약 UI 갱신
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	// Wave / Kill / Gold UI 갱신
	void UpdateWaveInfo(
		int32 CurrentWave,
		int32 CurrentKillCount,
		int32 TargetKillCount,
		int32 CurrentGold,
		int32 GoldPerKill
	);

	// 선택한 무기에 맞는 우측 하단 무기 UI와 Crosshair 갱신
	void RefreshWeaponUI();

	// 라운드 전환 메시지 표시
	void ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage);

	// 라운드 전환 메시지 숨김
	void HideRoundTransitionMessage();

	// 라운드 전환 메시지가 현재 표시 중인지 확인
	bool IsRoundTransitionMessageVisible() const;

	// HP 위험 피드백 제거
	void HideHPDangerFeedback();

	// 플레이어가 피격됐을 때 피격 알람 애니메이션 재생
	void PlayHitAlarm();

	// 게임 시작 시 페이드 인 / 준비 UI 애니메이션 재생
	void PlayGameStartTransition();

	// 보스 HP Bar 표시
	void ShowBossHPBar();

	// 화면 고정형 보스 HP Text
	// WBP_InGameUI의 TextBlock 이름이 BossHPText여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* BossHPText;

	// 보스 HP Bar 숨김
	void HideBossHPBar();

	// 보스 HP Bar 비율 갱신
	void UpdateBossHPBar(float CurrentHP, float MaxHP);

	void PlayKeyGuideUI();
	void HideKeyGuideUI();

	void PlayBossSkillCoolTimeUI();
	void HideBossSkillCoolTimeUI();

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayerHPText;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* HPDangerVignette;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon UI")
	UTexture2D* RifleCrosshairTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon UI")
	UTexture2D* ShotgunCrosshairTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon UI")
	UTexture2D* PistolCrosshairTexture;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* RifleWeaponUI;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* ShotgunWeaponUI;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* PistolWeaponUI;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WaveText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* KillText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* GoldPerKillText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* PlayerScoreText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* AIScoreText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionSubText;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* HitAlarmFrame;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* HitAlarmAnim;

	UPROPERTY(meta = (BindWidgetOptional))
	UImage* GameStartFadeImage;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* GameStartReadyText;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* GameStartAnim;

	// 화면 고정형 보스 HP Bar 전체 패널
	// WBP_InGameUI의 Border 이름이 BossHPPanel이어야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* BossHPPanel;

	// 화면 고정형 보스 HP ProgressBar
	// WBP_InGameUI의 ProgressBar 이름이 BossHPBar여야 연결된다.
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* BossHPBar;

	// 키 안내 UI 전체 패널
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> KeyGuidePanel;

	// 키 안내 UI FadeOut 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> KeyGuideFadeOutAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Key Guide")
	float KeyGuideVisibleDuration = 5.0f;

	// 보스 스킬 쿨타임 아이콘 패널
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> BossSkillCoolTimePanel;

	// 보스 스킬 쿨타임 10초 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> BossSkillCoolTimeAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss Skill")
	float BossSkillCoolTimeDuration = 10.0f;

private:
	void UpdateHPDangerFeedback(float CurrentHealth, float MaxHealth);
	void ApplyCrosshairTexture(UTexture2D* CrosshairTexture);

	float LastPlayerHealth = -1.0f;

	FTimerHandle KeyGuideFadeOutTimerHandle;
	void StartKeyGuideFadeOut();

	FTimerHandle BossSkillCoolTimeHideTimerHandle;
};