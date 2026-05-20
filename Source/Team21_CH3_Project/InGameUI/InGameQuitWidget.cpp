// InGameQuitWidget.cpp
#include "InGameQuitWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"


void UInGameQuitWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(quitButton)) quitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleQuitClicked);
	if (IsValid(settingButton)) settingButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingClicked);
	
	if (IsValid(popInAnim))
	{
		FWidgetAnimationDynamicEvent popInFinishedEvent;
		popInFinishedEvent.BindDynamic(this, &ThisClass::HandlePopInFinished);
		BindToAnimationFinished(popInAnim, popInFinishedEvent);
	}
	
	if (IsValid(popOutAnim))
	{
		FWidgetAnimationDynamicEvent popOutFinishedEvent;
		popOutFinishedEvent.BindDynamic(this, &ThisClass::HandlePopOutFinished);
		BindToAnimationFinished(popOutAnim, popOutFinishedEvent);
	}
	
	
	bIsOpening = false;
	currentState = EQuitWindowState::Closed;
	currentWidgetState = EWidgetState::Quit;
}

void UInGameQuitWidget::ShowWidget(EWidgetState widget){
	if (IsValid(screenSwitcher) == true)
	{
		currentWidgetState = widget;
		screenSwitcher->SetActiveWidgetIndex(int32(widget));
	}
}

void UInGameQuitWidget::HandleBackRequested(){
	if (bIsOpening) return;
	
	if (currentState == EQuitWindowState::Opened && currentWidgetState == EWidgetState::Setting)
	{
		ShowWidget(EWidgetState::Quit);
	}
	else if (currentState == EQuitWindowState::Closed && currentWidgetState == EWidgetState::Quit)
	{
		bIsOpening = true;
		if (IsValid(popInAnim) == true) PlayAnimation(popInAnim);
	}
	else if (currentState == EQuitWindowState::Opened && currentWidgetState == EWidgetState::Quit)
	{
		bIsOpening = true;
		if (IsValid(popOutAnim) == true) PlayAnimation(popOutAnim);
	}
}

void UInGameQuitWidget::HandleQuitClicked(){
	UGameplayStatics::OpenLevel(this, TEXT("OutGameMap"));
}

void UInGameQuitWidget::HandleSettingClicked(){
	ShowWidget(EWidgetState::Setting);
}

void UInGameQuitWidget::HandlePopInFinished(){
	SetVisibility(ESlateVisibility::Visible);
	currentState = EQuitWindowState::Opened;
	bIsOpening = false;
}

void UInGameQuitWidget::HandlePopOutFinished(){
	SetVisibility(ESlateVisibility::Collapsed);
	currentState = EQuitWindowState::Closed;
	bIsOpening = false;
}
