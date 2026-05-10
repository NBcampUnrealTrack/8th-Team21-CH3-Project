// OutGameQuitConfirmWidget.cpp
#include "OutGameQuitConfirmWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/WidgetAnimation.h"


void UOutGameQuitConfirmWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(confirmButton) == true) confirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleConfirmClicked);
	if (IsValid(cancelButton) == true) cancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCancelClicked);
	
	if (IsValid(FadeOutAnim) == true)
	{
		FWidgetAnimationDynamicEvent fadeOutFinishedEvent;
		fadeOutFinishedEvent.BindDynamic(this, &ThisClass::HandleFadeOutFinished);
		BindToAnimationFinished(FadeOutAnim, fadeOutFinishedEvent);
	}
	
	if (IsValid(FadeInAnim) == true)
	{
		FWidgetAnimationDynamicEvent fadeInFinishedEvent;
		fadeInFinishedEvent.BindDynamic(this, &ThisClass::HandleFadeInFinished);
		BindToAnimationFinished(FadeInAnim, fadeInFinishedEvent);
	}
	
	bIsPlay = false;
	State = EQuitConfirmState::Closed;
}

void UOutGameQuitConfirmWidget::ToggleQuitConfirm(){
	if (State == EQuitConfirmState::Closed)
	{
		ShowQuitConfirm();
	}
	else if (State == EQuitConfirmState::Opened)
	{
		HandleCancelClicked();
	}
}

void UOutGameQuitConfirmWidget::ShowQuitConfirm(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	SetVisibility(ESlateVisibility::Visible);
	if (IsValid(FadeOutAnim) == true) PlayAnimation(FadeOutAnim);
}

void UOutGameQuitConfirmWidget::HideQuitConfirm(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	if (IsValid(FadeOutAnim) == true) PlayAnimation(FadeOutAnim);
}

void UOutGameQuitConfirmWidget::HandleConfirmClicked(){
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UOutGameQuitConfirmWidget::HandleCancelClicked(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	if (IsValid(FadeInAnim) == true) PlayAnimation(FadeInAnim);
}

void UOutGameQuitConfirmWidget::HandleFadeOutFinished(){
	bIsPlay = false;
	State = EQuitConfirmState::Opened;
}

void UOutGameQuitConfirmWidget::HandleFadeInFinished(){
	bIsPlay = false;
	State = EQuitConfirmState::Closed;
	SetVisibility(ESlateVisibility::Collapsed);
}
