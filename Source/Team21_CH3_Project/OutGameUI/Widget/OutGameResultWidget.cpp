// OutGameResultWidget.cpp
#include "OutGameUI/Widget/OutGameResultWidget.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Game/TeamGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UOutGameResultWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(returnToLobbyButton) == true) returnToLobbyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleReturnToLobby);
	if (UTeamGameInstance* GameInstance = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		playerTotalKillText->SetText(FText::FromString(FString::Printf(TEXT("%d"), GameInstance->GetPlayerTotalKillCount())));
		if (GameInstance->GetIsWin() == true) gameResultText->SetText(FText::FromString(TEXT("Victory")));
		if (GameInstance->GetIsWin() == false) gameResultText->SetText(FText::FromString(TEXT("Defeat")));
	}
}

void UOutGameResultWidget::HandleReturnToLobby(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = Cast<UOutGameRootWidget>(PC->GetRootWidget()))
		{
			rootWidgetInstance->ShowTransition([rootWidgetInstance]
			{
				rootWidgetInstance->ShowWidget(EOutGameWidgetType::MainMenu);
				rootWidgetInstance->SetHeaderVisible(true);
				// gameInstance->SetIsWin(false);
			});
		}
	}
}
