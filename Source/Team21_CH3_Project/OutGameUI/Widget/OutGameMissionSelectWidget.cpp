// OutGameMissionSelect.cpp
#include "OutGameUI/Widget/OutGameMissionSelectWidget.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Components/Button.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameWeaponSelectWidget.h"

void UOutGameMissionSelectWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(EasyButton) == true) EasyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleEasyClicked);
	if (IsValid(NormalButton) == true) NormalButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleNormalClicked);
	if (IsValid(HardButton) == true) HardButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleHardClicked);
	if (IsValid(BackButton) == true) BackButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackClicked);
	
	selectedMapLevel = EMapLevel::Easy;
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
				pc->SetViewTargetByTag("WeaponSelectCamera", 0.0f);
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
	selectedMapLevel = EMapLevel::Normal;
	LevelClicked();
}

void UOutGameMissionSelectWidget::HandleHardClicked(){
	selectedMapLevel = EMapLevel::Hard;
	LevelClicked();
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

