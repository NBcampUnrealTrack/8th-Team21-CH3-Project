// OutGameMissionSelect.cpp
#include "OutGameUI/Widget/OutGameMissionSelectWidget.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameWeaponSelectWidget.h"
#include "Animation/WidgetAnimation.h"

void UOutGameMissionSelectWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(EasyButton) == true) EasyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleEasyClicked);
	if (IsValid(NormalButton) == true) NormalButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleNormalClicked);
	if (IsValid(HardButton) == true) HardButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleHardClicked);
	
	if (IsValid(fadeInAnim) == true)
	{
		FWidgetAnimationDynamicEvent fadeInFinishedEvent;
		fadeInFinishedEvent.BindDynamic(this, &ThisClass::HandleFadeInFinished);
		BindToAnimationFinished(fadeInAnim, fadeInFinishedEvent);
	}
	
	if (IsValid(fadeOutAnim) == true)
	{
		FWidgetAnimationDynamicEvent fadeOutFinishedEvent;
		fadeOutFinishedEvent.BindDynamic(this, &ThisClass::HandleFadeOutFinished);
		BindToAnimationFinished(fadeOutAnim, fadeOutFinishedEvent);
	}
	
	selectedMapLevel = EMapLevel::Easy;
	
	normalUnlockKillCount = 30;
	hardUnlockKillCount = 60;
	
	bIsOpening = false;
	
	if (IsValid(normalUnlockKillCountText) == true) normalUnlockKillCountText->SetText(
	FText::FromString(FString::Printf(TEXT("%d"), normalUnlockKillCount)));
	if (IsValid(hardUnlockKillCountText) == true) hardUnlockKillCountText->SetText(
		FText::FromString(FString::Printf(TEXT("%d"), hardUnlockKillCount)));
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
				pc->SetViewTargetByTag("FirstSelectCamera", 0.0f);
				rootWidgetInstance->ShowWidget(EOutGameWidgetType::WeaponSelect);
			});
		}
	}
}

void UOutGameMissionSelectWidget::PlayFadeInAnimation(){
	if (bIsOpening) return;
	bIsOpening = true;
	
	if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (IsValid(playerTotalKillText) == true) playerTotalKillText->SetText(
			FText::FromString(FString::Printf(TEXT("%d"), GI->GetPlayerTotalKillCount())));
	}
	
	if (IsValid(fadeInAnim) == true) PlayAnimation(fadeInAnim);
}

void UOutGameMissionSelectWidget::PlayFadeOutAnimation(){
	if (bIsOpening) return;
	bIsOpening = true;
	PlayUISound(EOutGameUISoundType::Back);
	if (IsValid(fadeOutAnim) == true) PlayAnimation(fadeOutAnim);
	
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

void UOutGameMissionSelectWidget::HandleFadeInFinished(){
	bIsOpening = false;
}

void UOutGameMissionSelectWidget::HandleFadeOutFinished(){
	bIsOpening = false;
	if (AOutGamePlayerController* pc = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* rootWidgetInstance = pc->GetRootWidget())
		{
			rootWidgetInstance->ShowWidget(EOutGameWidgetType::MainMenu);
		}
	}
}


