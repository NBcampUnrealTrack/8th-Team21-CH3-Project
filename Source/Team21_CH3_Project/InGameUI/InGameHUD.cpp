#include "InGameHUD.h"
#include "InGameUI.h"
#include "RoundTransitionWidget.h"
#include "Blueprint/UserWidget.h"

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (InGameUIClass)
	{
		InGameUIInstance = CreateWidget<UInGameUI>(GetWorld(), InGameUIClass);

		if (InGameUIInstance)
		{
			InGameUIInstance->AddToViewport();
		}
	}
}

void AInGameHUD::RefreshWaveUI(int32 CurrentWave, int32 CurrentKillCount, int32 TargetKillCount, int32 CurrentGold)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateWaveInfo(CurrentWave, CurrentKillCount, TargetKillCount, CurrentGold);
	}
}

void AInGameHUD::RefreshHealthUI(float CurrentHealth, float MaxHealth)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateHealth(CurrentHealth, MaxHealth);
	}
}

void AInGameHUD::RefreshAmmoUI(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateAmmo(CurrentAmmo, MaxAmmo);
	}
}

void AInGameHUD::ShowRoundTransitionUI(const FText& MainMessage, const FText& SubMessage)
{
	// 라운드 전환 UI가 표시될 때는 HP 위험 피드백을 제거한다.
	HideHPDangerFeedback();

	if (RoundTransitionWidgetInstance)
	{
		RoundTransitionWidgetInstance->SetRoundMessage(MainMessage, SubMessage);
		return;
	}

	if (!RoundTransitionWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RoundTransitionWidgetClass is not set in InGameHUD."));
		return;
	}

	RoundTransitionWidgetInstance = CreateWidget<URoundTransitionWidget>(GetWorld(), RoundTransitionWidgetClass);
	if (!RoundTransitionWidgetInstance)
	{
		return;
	}

	RoundTransitionWidgetInstance->SetRoundMessage(MainMessage, SubMessage);
	RoundTransitionWidgetInstance->AddToViewport(100);
}

void AInGameHUD::HideRoundTransitionUI()
{
	if (RoundTransitionWidgetInstance)
	{
		RoundTransitionWidgetInstance->RemoveFromParent();
		RoundTransitionWidgetInstance = nullptr;
	}
}

bool AInGameHUD::IsRoundTransitionUIShowing() const
{
	return RoundTransitionWidgetInstance != nullptr;
}

void AInGameHUD::HideHPDangerFeedback()
{
	if (InGameUIInstance)
	{
		InGameUIInstance->HideHPDangerFeedback();
	}
}