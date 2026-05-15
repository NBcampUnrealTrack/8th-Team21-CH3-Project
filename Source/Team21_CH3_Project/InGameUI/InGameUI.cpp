// InGameUI.cpp

#include "InGameUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	// UI가 처음 생성될 때 표시할 기본값
	// 실제 게임 중에는 HUD 또는 캐릭터에서 다시 갱신된다.
	UpdateHealth(100.f, 100.f);
	UpdateAmmo(30, 30);

	// Wave UI는 여기서 기본값으로 세팅하지 않는다.
	// 이유:
	// 레벨 리로드 후 GameMode에서 복구한 Wave / Kill / Gold 값을
	// NativeConstruct의 기본값이 다시 덮어쓸 수 있기 때문이다.
	//
	// Wave / Kill / Gold는 ShooterInGameMode -> InGameHUD -> UpdateWaveInfo 흐름으로만 갱신한다.

	// 라운드 전환 메시지는 처음에는 숨겨둔다.
	HideRoundTransitionMessage();

	// HP 위험 피드백도 처음에는 숨겨둔다.
	HideHPDangerFeedback();
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
		WaveText->SetText(FText::FromString(
			FString::Printf(TEXT("WAVE %d"), CurrentWave)
		));
	}

	if (KillText)
	{
		KillText->SetText(FText::FromString(
			FString::Printf(TEXT("KILL %d / %d"), CurrentKillCount, TargetKillCount)
		));
	}

	if (GoldText)
	{
		GoldText->SetText(FText::FromString(
			FString::Printf(TEXT("GOLD %d"), CurrentGold)
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