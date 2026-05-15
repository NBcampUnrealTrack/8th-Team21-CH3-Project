// UOutGameCommonHeaderWidget.cpp
#include "UOutGameCommonHeaderWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Game/TeamGameInstance.h"

void UUOutGameCommonHeaderWidget::NativeOnInitialized(){
	if (IsValid(playButton) == true) playButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandlePlayClicked);
	if (IsValid(weaponsButton) == true) weaponsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleWeaponsClicked);
	if (IsValid(storeButton) == true) storeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleStoreButton);
	if (IsValid(settingsButton) == true) settingsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingsClicked);
	if (IsValid(traitButton) == true) traitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleTraitClicked);
	
	UpdateCommonUI();
}

void UUOutGameCommonHeaderWidget::SetActiveTab(EOutGameWidgetType widgetType){
	//SetButtonSelected(playButton, widgetType == EOutGameWidgetType::MainMenu);
	//SetButtonSelected(settingsButton, widgetType == EOutGameWidgetType::Settings);
	//SetButtonSelected(storeButton, widgetType == EOutGameWidgetType::Store);
	//SetButtonSelected(weaponsButton, widgetType == EOutGameWidgetType::WeaponPreview);

	
	SetSelectedVisible(playSelectedImage, widgetType == EOutGameWidgetType::MainMenu);
	SetSelectedVisible(weaponsSelectedImage, widgetType == EOutGameWidgetType::WeaponPreview);
	SetSelectedVisible(storeSelectedImage, widgetType == EOutGameWidgetType::Store);
	SetSelectedVisible(settingsSelectedImage, widgetType == EOutGameWidgetType::Settings);
	//SetSelectedVisible(traitSelectedImage, widgetType == EOutGameWidgetType::Trait);
}

void UUOutGameCommonHeaderWidget::UpdateCommonUI(){
	if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (IsValid(playerTotalKillCount) == true) playerTotalKillCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), GI->GetPlayerTotalKillCount())));
		if (IsValid(playerGold) == true) playerGold->SetText(FText::FromString(FString::Printf(TEXT("%d G"), GI->GetPlayerGold())));
	}
}

void UUOutGameCommonHeaderWidget::SetSelectedVisible(UImage* image, bool bSelected){
	/*
	if (IsValid(button) == false) return;
	
	button->SetBackgroundColor(
		bSelected
			? FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)
			: FLinearColor(0.45f, 0.45f, 0.45f, 1.0f)
	);
	*/
	if (IsValid(image) == false) return;
	
	image->SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
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

void UUOutGameCommonHeaderWidget::HandleTraitClicked(){  }
