// OutGameMissionSelect.cpp
#include "OutGameUI/Widget/OutGameMissionSelectWidget.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameWeaponSelectWidget.h"

void UOutGameMissionSelectWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(EasyButton) == true) EasyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleEasyClicked);
	if (IsValid(NormalButton) == true) NormalButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleNormalClicked);
	if (IsValid(HardButton) == true) HardButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleHardClicked);
	if (IsValid(BackButton) == true) BackButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackClicked);
	
	selectedMapLevel = EMapLevel::Easy;
	
	normalUnlockKillCount = 30;
	hardUnlockKillCount = 60;
	
	if (IsValid(normalUnlockKillCountText) == true) normalUnlockKillCountText->SetText(
	FText::FromString(FString::Printf(TEXT("%d"), normalUnlockKillCount)));
	if (IsValid(hardUnlockKillCountText) == true) hardUnlockKillCountText->SetText(
		FText::FromString(FString::Printf(TEXT("%d"), hardUnlockKillCount)));
	
	if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (IsValid(playerTotalKillText) == true) playerTotalKillText->SetText(
			FText::FromString(FString::Printf(TEXT("%d"), GI->GetPlayerTotalKillCount())));
	}
	
}

void UOutGameMissionSelectWidget::LevelClicked(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			rootWidgetInstance->ShowTransition([this, pc, rootWidgetInstance]()
			{
				rootWidgetInstance->SetSelectedLevel(selectedMapLevel);
				rootWidgetInstance->SetHeaderVisible(false);
				pc->SetViewTargetByTag("RifleSelectCamera", 0.0f);
				rootWidgetInstance->ShowWidget(EOutGameWidgetType::WeaponSelect);
			});
		}
	}
}

void UOutGameMissionSelectWidget::HandleEasyClicked(){
	selectedMapLevel = EMapLevel::Easy;
	LevelClicked();
}

void UOutGameMissionSelectWidget::HandleNormalClicked(){
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false) return;
	
	if (normalUnlockKillCount <= GI->GetPlayerTotalKillCount())
	{
		selectedMapLevel = EMapLevel::Normal;
		LevelClicked();
	}
	else
	{
		if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
		{
			if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
			{
				rootWidgetInstance->ShowMissionSelectConfirm();
			}
		}
	}

}

void UOutGameMissionSelectWidget::HandleHardClicked(){
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false) return;
	
	if (hardUnlockKillCount <= GI->GetPlayerTotalKillCount())
	{
		selectedMapLevel = EMapLevel::Hard;
		LevelClicked();
	}
	else
	{
		if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
		{
			if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
			{
				rootWidgetInstance->ShowMissionSelectConfirm();
			}
		}
	}

}

void UOutGameMissionSelectWidget::HandleBackClicked(){
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			rootWidgetInstance->ShowWidget(EOutGameWidgetType::MainMenu);
			// rootWidgetInstance->ShowTransition([this, pc, rootWidgetInstance](){}
		}
	}
}

