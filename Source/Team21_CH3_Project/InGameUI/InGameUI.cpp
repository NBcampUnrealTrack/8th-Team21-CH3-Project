// InGameUI.cpp

#include "InGameUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Widget.h"
#include "Engine/Texture2D.h"
#include "Game/TeamGameInstance.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateHealth(100.f, 100.f);
	UpdateAmmo(30, 30);

	RefreshWeaponUI();

	HideRoundTransitionMessage();
	HideHPDangerFeedback();
	HideBossHPBar();
	HideKeyGuideUI();

	if (HitAlarmFrame)
	{
		HitAlarmFrame->SetVisibility(ESlateVisibility::Collapsed);
		HitAlarmFrame->SetRenderOpacity(0.0f);
	}

	if (GameStartFadeImage)
	{
		GameStartFadeImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		GameStartFadeImage->SetRenderOpacity(1.0f);
	}

	if (GameStartReadyText)
	{
		GameStartReadyText->SetVisibility(ESlateVisibility::HitTestInvisible);
		GameStartReadyText->SetRenderOpacity(0.0f);
	}
}

void UInGameUI::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (MaxHealth <= 0.f)
	{
		if (HealthBar)
		{
			HealthBar->SetPercent(0.f);
		}

		if (PlayerHPText)
		{
			PlayerHPText->SetText(FText::FromString(TEXT("0 / 0")));
		}

		HideHPDangerFeedback();
		return;
	}

	const float SafeHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	const float HealthPercent = SafeHealth / MaxHealth;

	if (LastPlayerHealth >= 0.0f && SafeHealth < LastPlayerHealth)
	{
		PlayHitAlarm();
	}

	LastPlayerHealth = SafeHealth;

	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}

	if (PlayerHPText)
	{
		const FString HPTextString = FString::Printf(
			TEXT("%d / %d"),
			FMath::RoundToInt(SafeHealth),
			FMath::RoundToInt(MaxHealth)
		);

		PlayerHPText->SetText(FText::FromString(HPTextString));
	}

	UpdateHPDangerFeedback(SafeHealth, MaxHealth);
}

void UInGameUI::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!AmmoText)
	{
		return;
	}

	const FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
	AmmoText->SetText(FText::FromString(AmmoString));
}

void UInGameUI::RefreshWeaponUI()
{
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());

	const EWeaponType SelectedWeaponType = GI
		? GI->GetSelectedWeaponType()
		: EWeaponType::Rifle;

	if (RifleWeaponUI)
	{
		RifleWeaponUI->SetVisibility(
			SelectedWeaponType == EWeaponType::Rifle
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
		);
	}

	if (ShotgunWeaponUI)
	{
		ShotgunWeaponUI->SetVisibility(
			SelectedWeaponType == EWeaponType::Shotgun
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
		);
	}

	if (PistolWeaponUI)
	{
		PistolWeaponUI->SetVisibility(
			SelectedWeaponType == EWeaponType::Pistol
			? ESlateVisibility::HitTestInvisible
			: ESlateVisibility::Collapsed
		);
	}

	switch (SelectedWeaponType)
	{
	case EWeaponType::Rifle:
		ApplyCrosshairTexture(RifleCrosshairTexture);
		break;

	case EWeaponType::Shotgun:
		ApplyCrosshairTexture(ShotgunCrosshairTexture);
		break;

	case EWeaponType::Pistol:
		ApplyCrosshairTexture(PistolCrosshairTexture);
		break;

	default:
		ApplyCrosshairTexture(RifleCrosshairTexture);
		break;
	}
}

void UInGameUI::ApplyCrosshairTexture(UTexture2D* CrosshairTexture)
{
	if (!CrosshairImage || !CrosshairTexture)
	{
		return;
	}

	CrosshairImage->SetBrushFromTexture(CrosshairTexture, true);
}

void UInGameUI::UpdateWaveInfo(
	int32 CurrentWave,
	int32 CurrentKillCount,
	int32 TargetKillCount,
	int32 CurrentGold,
	int32 GoldPerKill
)
{
	if (WaveText)
	{
		if (CurrentWave == 4)
		{
			WaveText->SetText(FText::FromString(TEXT("BOSS")));
		}
		else
		{
			WaveText->SetText(FText::FromString(
				FString::Printf(TEXT("WAVE %d"), CurrentWave)
			));
		}
	}

	if (KillText)
	{
		KillText->SetText(FText::FromString(
			FString::Printf(TEXT("%d / %d"), CurrentKillCount, TargetKillCount)
		));
	}

	if (GoldText)
	{
		GoldText->SetText(FText::FromString(
			FString::Printf(TEXT("%dG"), CurrentGold)
		));
	}

	if (GoldPerKillText)
	{
		GoldPerKillText->SetText(FText::FromString(
			FString::Printf(TEXT("GOLD/KILL +%d"), GoldPerKill)
		));
	}
}

void UInGameUI::ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage)
{
	HideHPDangerFeedback();

	const FText SafeMainMessage = MainMessage.IsEmpty()
		? FText::FromString(TEXT("Next Round"))
		: MainMessage;

	if (RoundTransitionText)
	{
		RoundTransitionText->SetText(SafeMainMessage);
		RoundTransitionText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (RoundTransitionSubText)
	{
		RoundTransitionSubText->SetText(SubMessage);
		RoundTransitionSubText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UInGameUI::HideRoundTransitionMessage()
{
	if (RoundTransitionText)
	{
		RoundTransitionText->SetText(FText::GetEmpty());
		RoundTransitionText->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (RoundTransitionSubText)
	{
		RoundTransitionSubText->SetText(FText::GetEmpty());
		RoundTransitionSubText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UInGameUI::IsRoundTransitionMessageVisible() const
{
	if (!RoundTransitionText)
	{
		return false;
	}

	return RoundTransitionText->GetVisibility() != ESlateVisibility::Collapsed;
}

void UInGameUI::UpdateHPDangerFeedback(float CurrentHealth, float MaxHealth)
{
	if (!HPDangerVignette)
	{
		return;
	}

	if (MaxHealth <= 0.f || CurrentHealth < 0.f)
	{
		HideHPDangerFeedback();
		return;
	}

	if (CurrentHealth <= 0.f)
	{
		HideHPDangerFeedback();
		return;
	}

	const float DangerThreshold = MaxHealth * 0.3f;

	if (CurrentHealth <= DangerThreshold)
	{
		HPDangerVignette->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		HideHPDangerFeedback();
	}
}

void UInGameUI::HideHPDangerFeedback()
{
	if (HPDangerVignette)
	{
		HPDangerVignette->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInGameUI::PlayHitAlarm()
{
	if (HitAlarmFrame)
	{
		HitAlarmFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (HitAlarmAnim)
	{
		StopAnimation(HitAlarmAnim);
		PlayAnimation(HitAlarmAnim);
	}
}

void UInGameUI::PlayGameStartTransition()
{
	if (GameStartFadeImage)
	{
		GameStartFadeImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (GameStartReadyText)
	{
		GameStartReadyText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (GameStartAnim)
	{
		StopAnimation(GameStartAnim);
		PlayAnimation(GameStartAnim);
	}
}

void UInGameUI::ShowBossHPBar()
{
	if (BossHPPanel)
	{
		BossHPPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (BossHPBar)
	{
		BossHPBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		BossHPBar->SetFillColorAndOpacity(FLinearColor::White);
		BossHPBar->SetPercent(1.0f);
	}

	if (BossHPText)
	{
		BossHPText->SetVisibility(ESlateVisibility::HitTestInvisible);
		BossHPText->SetText(FText::GetEmpty());
	}
}

void UInGameUI::HideBossHPBar()
{
	if (BossHPPanel)
	{
		BossHPPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (BossHPBar)
	{
		BossHPBar->SetVisibility(ESlateVisibility::Collapsed);
		BossHPBar->SetPercent(0.0f);
	}

	if (BossHPText)
	{
		BossHPText->SetVisibility(ESlateVisibility::Collapsed);
		BossHPText->SetText(FText::GetEmpty());
	}
}

void UInGameUI::UpdateBossHPBar(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.0f)
	{
		HideBossHPBar();
		return;
	}

	const float SafeCurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);
	const float HPPercent = SafeCurrentHP / MaxHP;

	if (BossHPBar)
	{
		BossHPBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		BossHPBar->SetFillColorAndOpacity(FLinearColor::White);
		BossHPBar->SetPercent(FMath::Clamp(HPPercent, 0.0f, 1.0f));
	}

	if (BossHPText)
	{
		BossHPText->SetVisibility(ESlateVisibility::HitTestInvisible);

		const FString HPTextString = FString::Printf(
			TEXT("%d"),
			FMath::RoundToInt(SafeCurrentHP)
		);

		BossHPText->SetText(FText::FromString(HPTextString));
	}

	if (SafeCurrentHP <= 0.0f)
	{
		HideBossHPBar();
	}
}

void UInGameUI::PlayKeyGuideUI()
{
	if (!KeyGuidePanel)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(KeyGuideFadeOutTimerHandle);

	if (KeyGuideFadeOutAnim)
	{
		StopAnimation(KeyGuideFadeOutAnim);
	}

	// 처음에 바로 보이지 않게 0으로 시작
	KeyGuidePanel->SetRenderOpacity(0.0f);
	KeyGuidePanel->SetVisibility(ESlateVisibility::HitTestInvisible);

	// 네가 만든 Fade In → 유지 → Fade Out 애니메이션을 바로 재생
	if (KeyGuideFadeOutAnim)
	{
		PlayAnimation(KeyGuideFadeOutAnim);
	}
}

void UInGameUI::StartKeyGuideFadeOut()
{
	if (!KeyGuidePanel)
	{
		return;
	}

	if (KeyGuideFadeOutAnim)
	{
		StopAnimation(KeyGuideFadeOutAnim);
		PlayAnimation(KeyGuideFadeOutAnim);
		return;
	}

	HideKeyGuideUI();
}

void UInGameUI::HideKeyGuideUI()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(KeyGuideFadeOutTimerHandle);
	}

	if (KeyGuidePanel)
	{
		KeyGuidePanel->SetRenderOpacity(0.0f);
		KeyGuidePanel->SetVisibility(ESlateVisibility::Collapsed);
	}
}