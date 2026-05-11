// InGameHUD.cpp

#include "InGameHUD.h"
#include "InGameUI.h"
#include "RoundTransitionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Game/TeamGameInstance.h"

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (InGameUIClass)
	{
		InGameUIInstance = CreateWidget<UInGameUI>(GetWorld(), InGameUIClass);

		if (InGameUIInstance)
		{
			InGameUIInstance->AddToViewport();
			RefreshMatchUIFromGameInstance();
		}
	}
}

void AInGameHUD::RefreshMatchUIFromGameInstance()
{
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (!GI) return;

	const int32 PlayerScore = GI->GetPlayerScore();
	const int32 AIScore = GI->GetAIScore();
	const int32 CurrentRound = PlayerScore + AIScore + 1;

	RefreshMatchUI(PlayerScore, AIScore, CurrentRound);
}

void AInGameHUD::RefreshMatchUI(int32 PlayerScore, int32 AIScore, int32 Round)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateMatchInfo(PlayerScore, AIScore, Round);
	}
}

void AInGameHUD::RefreshHealthUI(float CurrentHealth, float MaxHealth)
{
	if (InGameUIInstance)
	{
		InGameUIInstance->UpdateHealth(CurrentHealth, MaxHealth);
	}
}

void AInGameHUD::ShowRoundTransitionUI(const FText& MainMessage, const FText& SubMessage)
{
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
	if (!RoundTransitionWidgetInstance) return;

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