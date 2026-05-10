// UOutGameCommonHeaderWidget.cpp
#include "UOutGameCommonHeaderWidget.h"
#include "Components/Button.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"

void UUOutGameCommonHeaderWidget::NativeOnInitialized(){
	if (IsValid(playButton) == true) playButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandlePlayClicked);
	if (IsValid(weaponsButton) == true) weaponsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleWeaponsClicked);
	if (IsValid(storeButton) == true) storeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleStoreButton);
	if (IsValid(settingsButton) == true) settingsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingsClicked);
}

void UUOutGameCommonHeaderWidget::HandlePlayClicked(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* RootWidgetInstance = PC->GetRootWidget())
		{
			RootWidgetInstance->ShowWidget(EOutGameWidgetType::MainMenu);
			PC->SetViewTargetByTag("LobbyCamera", 0.0f);
		}
	}
}

void UUOutGameCommonHeaderWidget::HandleWeaponsClicked(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* RootWidgetInstance = PC->GetRootWidget())
		{
			RootWidgetInstance->ShowWidget(EOutGameWidgetType::WeaponPreview);
			PC->SetViewTargetByTag("SettingsCamera", 0.0f);
		}
	}
}

void UUOutGameCommonHeaderWidget::HandleStoreButton(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* RootWidgetInstance = PC->GetRootWidget())
		{
			RootWidgetInstance->ShowWidget(EOutGameWidgetType::Store);
			PC->SetViewTargetByTag("SettingsCamera", 0.0f);
		}
	}
}

void UUOutGameCommonHeaderWidget::HandleSettingsClicked(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* RootWidgetInstance = PC->GetRootWidget())
		{
			RootWidgetInstance->ShowWidget(EOutGameWidgetType::Settings);
			PC->SetViewTargetByTag("SettingsCamera", 0.0f);
		}
	}
}
