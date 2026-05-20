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
	if (bIsOpening)
	{
		UE_LOG(LogTemp, Warning, TEXT("bIsOpening is true"));
		return;
	}
	APlayerController* PC = GetOwningPlayer();
	if (IsValid(PC) == false) return;
	
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false); // Drag and Click -> don't hide cursor
	PC->SetInputMode(inputMode);
	PC->bShowMouseCursor = true;
	
	if (currentState == EQuitWindowState::Opened && currentWidgetState == EWidgetState::Setting)
	{
		ShowWidget(EWidgetState::Quit);
	}
	else if (currentState == EQuitWindowState::Closed && currentWidgetState == EWidgetState::Quit)
	{
		bIsOpening = true;
		SetVisibility(ESlateVisibility::Visible);
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		if (IsValid(popInAnim) == true) PlayAnimation(popInAnim);
	}
	else if (currentState == EQuitWindowState::Opened && currentWidgetState == EWidgetState::Quit)
	{
		bIsOpening = true;
		
		if (IsValid(PC) == false) return;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		FInputModeGameOnly inputModeGameOnly;
		PC->SetInputMode(inputModeGameOnly);
		PC->bShowMouseCursor = false;
	

		if (IsValid(popOutAnim) == true) {
			PlayAnimation(popOutAnim);
			UE_LOG(LogTemp, Warning, TEXT("popOutAnim On"));
		}
	}
}

void UInGameQuitWidget::HandleQuitClicked(){
	UGameplayStatics::OpenLevel(this, TEXT("OutGameMap"));
}

void UInGameQuitWidget::HandleSettingClicked(){
	ShowWidget(EWidgetState::Setting);
}

void UInGameQuitWidget::HandlePopInFinished(){

	currentState = EQuitWindowState::Opened;
	bIsOpening = false;
}

void UInGameQuitWidget::HandlePopOutFinished(){
	UE_LOG(LogTemp, Warning, TEXT("popOutAnim Finished"));
	bIsOpening = false;
	SetVisibility(ESlateVisibility::Collapsed);
	currentState = EQuitWindowState::Closed;
}
