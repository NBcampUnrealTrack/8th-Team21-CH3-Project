// InGameUI.cpp

#include "InGameUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateHealth(50.f, 100.f);
	UpdateAmmo(30, 30);

	HideRoundTransitionMessage();
}

void UInGameUI::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (!HealthBar) return;

	if (MaxHealth <= 0.f)
	{
		HealthBar->SetPercent(0.f);
		return;
	}

	const float SafeHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
	HealthBar->SetPercent(SafeHealth / MaxHealth);
}

void UInGameUI::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!AmmoText) return;

	const FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
	AmmoText->SetText(FText::FromString(AmmoString));
}

void UInGameUI::UpdateMatchInfo(int32 PlayerScore, int32 AIScore, int32 Round)
{
	if (PlayerScoreText)
	{
		PlayerScoreText->SetText(FText::AsNumber(PlayerScore));
	}

	if (AIScoreText)
	{
		AIScoreText->SetText(FText::AsNumber(AIScore));
	}

	if (RoundText)
	{
		RoundText->SetText(FText::FromString(FString::Printf(TEXT("ROUND %d"), Round)));
	}
}

void UInGameUI::ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage)
{
	const FText SafeMainMessage = MainMessage.IsEmpty()
		? FText::FromString(TEXT("Next Round"))
		: MainMessage;

	const FText SafeSubMessage = SubMessage.IsEmpty()
		? FText::FromString(TEXT("Get Ready for the Next Round"))
		: SubMessage;

	if (RoundTransitionText)
	{
		RoundTransitionText->SetText(SafeMainMessage);
		RoundTransitionText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RoundTransitionText is nullptr. Check widget name."));
	}

	if (RoundTransitionSubText)
	{
		RoundTransitionSubText->SetText(SafeSubMessage);
		RoundTransitionSubText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RoundTransitionSubText is nullptr. Check widget name."));
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
	if (RoundTransitionText && RoundTransitionText->GetVisibility() != ESlateVisibility::Collapsed)
	{
		return true;
	}

	if (RoundTransitionSubText && RoundTransitionSubText->GetVisibility() != ESlateVisibility::Collapsed)
	{
		return true;
	}

	return false;
}