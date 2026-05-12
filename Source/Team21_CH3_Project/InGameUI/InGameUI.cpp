// InGameUI.cpp

#include "InGameUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	// UI가 처음 생성될 때 표시할 기본값
	// 실제 게임 중에는 HUD 또는 캐릭터에서 RefreshHealthUI를 통해 다시 갱신된다.
	UpdateHealth(100.f, 100.f);
	UpdateAmmo(30, 30);

	// 점수 / 라운드는 GameInstance 값을 기준으로 HUD에서 갱신하므로
	// 여기서 0점, 1라운드로 강제 초기화하지 않는다.

	// 라운드 전환 메시지는 처음에는 숨겨둔다.
	HideRoundTransitionMessage();

	// HP 위험 피드백도 처음에는 숨겨둔다.
	HideHPDangerFeedback();
}

void UInGameUI::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	// MaxHealth가 잘못된 값이면 HP Bar, Text, 위험 피드백을 안전하게 0 처리한다.
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

	// CurrentHealth가 0 ~ MaxHealth 범위를 벗어나지 않도록 보정
	const float SafeHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

	// ProgressBar는 0.0 ~ 1.0 값을 사용하므로 비율로 변환
	const float HealthPercent = SafeHealth / MaxHealth;

	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}

	// HP Bar 위에 표시될 숫자 Text 갱신
	// 예: 75 / 100
	if (PlayerHPText)
	{
		const FString HPTextString = FString::Printf(
			TEXT("%d / %d"),
			FMath::RoundToInt(SafeHealth),
			FMath::RoundToInt(MaxHealth)
		);

		PlayerHPText->SetText(FText::FromString(HPTextString));
	}

	// HP 위험 피드백 갱신
	UpdateHPDangerFeedback(SafeHealth, MaxHealth);
}

void UInGameUI::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!AmmoText)
	{
		return;
	}

	// 탄약 표시 형식
	// 예: 30 / 30
	const FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
	AmmoText->SetText(FText::FromString(AmmoString));
}

void UInGameUI::UpdateMatchInfo(int32 PlayerScore, int32 AIScore, int32 Round)
{
	// 플레이어 점수 갱신
	if (PlayerScoreText)
	{
		PlayerScoreText->SetText(FText::AsNumber(PlayerScore));
	}

	// AI 점수 갱신
	if (AIScoreText)
	{
		AIScoreText->SetText(FText::AsNumber(AIScore));
	}

	// 현재 라운드 갱신
	// 예: ROUND 2
	if (RoundText)
	{
		RoundText->SetText(FText::FromString(FString::Printf(TEXT("ROUND %d"), Round)));
	}
}

void UInGameUI::ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage)
{
	// 라운드 전환 UI 또는 결과성 메시지가 표시될 때는 위험 피드백을 제거한다.
	HideHPDangerFeedback();

	// 메인 메시지가 없으면 기본 문구로 Next Round 표시
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
	// 라운드 전환 메인 메시지 숨김
	if (RoundTransitionText)
	{
		RoundTransitionText->SetText(FText::GetEmpty());
		RoundTransitionText->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 라운드 전환 서브 메시지 숨김
	if (RoundTransitionSubText)
	{
		RoundTransitionSubText->SetText(FText::GetEmpty());
		RoundTransitionSubText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UInGameUI::IsRoundTransitionMessageVisible() const
{
	// 메인 메시지가 표시 중이면 라운드 전환 UI가 열려 있다고 판단한다.
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

	// HP 값이 비정상적이면 위험 피드백을 표시하지 않는다.
	if (MaxHealth <= 0.f || CurrentHealth < 0.f)
	{
		HideHPDangerFeedback();
		return;
	}

	// 플레이어가 사망한 경우에는 Result UI 또는 Death 처리가 우선되므로 위험 피드백을 제거한다.
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