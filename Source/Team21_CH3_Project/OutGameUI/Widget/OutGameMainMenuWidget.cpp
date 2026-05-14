// OutGameMainMenuWidget.cpp
#include "OutGameUI/Widget/OutGameMainMenuWidget.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

void UOutGameMainMenuWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();

	if (IsValid(NewGameButton) == true)
		NewGameButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleNewGameClicked);
	if (IsValid(ContinueButton) == true)
		ContinueButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleContinueClicked);
	if (IsValid(PlayButton) == true)
		PlayButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandlePlayClicked);
	if (IsValid(QuitButton) == true)
		QuitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitClicked);

}

void UOutGameMainMenuWidget::ShowLobby(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			if (IsValid(ScreenSwitcher) == true) { ScreenSwitcher->SetActiveWidgetIndex(1); }
		}
	}
}

bool UOutGameMainMenuWidget::IsLobby() const{
	if (IsValid(ScreenSwitcher) == false) return false;
	
	return ScreenSwitcher->GetActiveWidgetIndex() == 1;
}

void UOutGameMainMenuWidget::HandleNewGameClicked(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			rootWidgetInstance->ShowNewGameConfirm();
		}
	}
}

void UOutGameMainMenuWidget::HandleContinueClicked(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			rootWidgetInstance->ShowTransition([rootWidgetInstance]
			{
				rootWidgetInstance->ShowLobby();
			});
		}
	}
}

void UOutGameMainMenuWidget::HandlePlayClicked(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			rootWidgetInstance->ShowWidget(EOutGameWidgetType::MissionSelect);
			// rootWidgetInstance->ShowTransition([this, pc, rootWidgetInstance]() {};
		}
	}
}

void UOutGameMainMenuWidget::HandleQuitClicked(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		UKismetSystemLibrary::QuitGame(this, pc, EQuitPreference::Quit, false);
	}
}
