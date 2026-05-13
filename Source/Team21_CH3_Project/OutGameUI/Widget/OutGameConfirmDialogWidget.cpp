// OutGameConfirmDialogWidget.cpp
#include "OutGameConfirmDialogWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/WidgetAnimation.h"


void UOutGameConfirmDialogWidget::NativeOnInitialized(){
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

void UOutGameConfirmDialogWidget::ToggleQuitConfirm(){
	if (State == EQuitConfirmState::Closed)
	{
		ShowQuitConfirm();
	}
	else if (State == EQuitConfirmState::Opened)
	{
		HandleCancelClicked();
	}
}

void UOutGameConfirmDialogWidget::ShowQuitConfirm(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	SetVisibility(ESlateVisibility::Visible);
	if (IsValid(FadeOutAnim) == true) PlayAnimation(FadeOutAnim);
}

void UOutGameConfirmDialogWidget::HideQuitConfirm(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	if (IsValid(FadeOutAnim) == true) PlayAnimation(FadeOutAnim);
}

void UOutGameConfirmDialogWidget::HandleConfirmClicked(){
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UOutGameConfirmDialogWidget::HandleCancelClicked(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	if (IsValid(FadeInAnim) == true) PlayAnimation(FadeInAnim);
}

void UOutGameConfirmDialogWidget::HandleFadeOutFinished(){
	bIsPlay = false;
	State = EQuitConfirmState::Opened;
}

void UOutGameConfirmDialogWidget::HandleFadeInFinished(){
	bIsPlay = false;
	State = EQuitConfirmState::Closed;
	SetVisibility(ESlateVisibility::Collapsed);
}
