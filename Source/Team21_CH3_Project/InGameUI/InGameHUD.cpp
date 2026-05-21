// InGameHUD.cpp

#include "InGameHUD.h"
#include "InGameUI.h"
#include "RoundTransitionWidget.h"
#include "ShooterInGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/Widget/OutGameTransitionWidget.h"

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (InGameUIClass)
	{
		InGameUIInstance = CreateWidget<UInGameUI>(GetWorld(), InGameUIClass);

		if (InGameUIInstance)
		{
			InGameUIInstance->AddToViewport();

			if (InGameStartTransitionWidgetClass)
			{
				InGameStartTransitionWidgetInstance = CreateWidget<UOutGameTransitionWidget>(
					GetWorld(),
					InGameStartTransitionWidgetClass
				);

				if (InGameStartTransitionWidgetInstance)
				{
					InGameStartTransitionWidgetInstance->AddToViewport(10000);
					InGameStartTransitionWidgetInstance->PlayFadeIn();
				}
			}

			// OutGame -> InGame 진입 FadeIn이 끝난 뒤 READY / SET / START 연출을 재생한다.
			GetWorldTimerManager().ClearTimer(InGameStartReadyTimerHandle);

			GetWorldTimerManager().SetTimer(
				InGameStartReadyTimerHandle,
				this,
				&AInGameHUD::PlayGameStartReadyTransition,
				InGameStartReadyDelay,
				false
			);

			// HUD 위젯 생성이 완료된 직후 GameMode에 Wave/Kill/Gold UI 갱신을 다시 요청한다.
			AShooterInGameMode* GameMode = Cast<AShooterInGameMode>(UGameplayStatics::GetGameMode(this));
			if (GameMode)
			{
				GameMode->RequestHUDWaveInfoRefreshRetry();
			}
		}
	}
}

void AInGameHUD::RefreshWaveUI(
	int32 CurrentWave,
	int32 CurrentKillCount,
	int32 TargetKillCount,
	int32 CurrentGold,
	int32 GoldPerKill
)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateWaveInfo(
			CurrentWave,
			CurrentKillCount,
			TargetKillCount,
			CurrentGold,
			GoldPerKill
		);
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

void AInGameHUD::PlayGameStartReadyTransition()
{
	if (InGameUIInstance)
	{
		InGameUIInstance->PlayGameStartTransition();
	}
}

void AInGameHUD::PlayLevelTransitionFadeOut()
{
	if (!InGameStartTransitionWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameStartTransitionWidgetClass is not set in InGameHUD."));
		return;
	}

	if (!InGameStartTransitionWidgetInstance)
	{
		InGameStartTransitionWidgetInstance = CreateWidget<UOutGameTransitionWidget>(
			GetWorld(),
			InGameStartTransitionWidgetClass
		);

		if (InGameStartTransitionWidgetInstance)
		{
			InGameStartTransitionWidgetInstance->AddToViewport(10000);
		}
	}

	if (InGameStartTransitionWidgetInstance)
	{
		// 웨이브 전환 / 레벨 리로드 직전 화면을 검게 덮는다.
		InGameStartTransitionWidgetInstance->PlayFadeOut();
	}
}

void AInGameHUD::ShowBossHPBar()
{
	if (InGameUIInstance)
	{
		InGameUIInstance->ShowBossHPBar();
	}
}

void AInGameHUD::HideBossHPBar()
{
	if (InGameUIInstance)
	{
		InGameUIInstance->HideBossHPBar();
	}
}

void AInGameHUD::UpdateBossHPBar(float CurrentHP, float MaxHP)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateBossHPBar(CurrentHP, MaxHP);
	}
}