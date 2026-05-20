// OutGameConfirmDialogWidget.cpp
#include "OutGameConfirmDialogWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UOutGameConfirmDialogWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(confirmButton) == true) confirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleConfirmClicked);
	if (IsValid(cancelButton) == true) cancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCancelClicked);
	if (IsValid(okButton) == true) okButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleOkClicked);
	
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
	SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(okButton) == true) okButton->SetVisibility(ESlateVisibility::Collapsed);
}

bool UOutGameConfirmDialogWidget::IsOpend(){
	return State == EQuitConfirmState::Opened || State == EQuitConfirmState::Opening;
}

void UOutGameConfirmDialogWidget::ShowConfirmDialog(const FText& InTitle, const FText& InMessage){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	if (IsValid(titleText) == true) titleText->SetText(InTitle);
	if (IsValid(messageText) == true) messageText->SetText(InMessage);
	
	pendingTitleText = InTitle;
	pendingMessageText = InMessage;
	SetVisibility(ESlateVisibility::Visible);
	if (IsValid(FadeOutAnim) == true) PlayAnimation(FadeOutAnim);
}

void UOutGameConfirmDialogWidget::EnableOkButton(){
	if (IsValid(confirmButton) == true) confirmButton->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(cancelButton) == true) cancelButton->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(okButton) == true) okButton->SetVisibility(ESlateVisibility::Visible);
}

void UOutGameConfirmDialogWidget::HideConfirmDialog(){
	if (bIsPlay == true) return;
	bIsPlay = true;
	
	if (IsValid(FadeInAnim) == true) PlayAnimation(FadeInAnim);
}

void UOutGameConfirmDialogWidget::HandleConfirmClicked(){
	HideConfirmDialog();
	OnConfirmed.Broadcast();
}

void UOutGameConfirmDialogWidget::HandleCancelClicked(){
	HideConfirmDialog();
	OnCanceled.Broadcast();
}

void UOutGameConfirmDialogWidget::HandleOkClicked(){
	HideConfirmDialog();
	OnCanceled.Broadcast();
}

void UOutGameConfirmDialogWidget::HandleFadeOutFinished(){
	bIsPlay = false;
	State = EQuitConfirmState::Opened;
}

void UOutGameConfirmDialogWidget::HandleFadeInFinished(){
	bIsPlay = false;
	State = EQuitConfirmState::Closed;
	SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(okButton) == true) okButton->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(confirmButton) == true) confirmButton->SetVisibility(ESlateVisibility::Visible);
	if (IsValid(cancelButton) == true) cancelButton->SetVisibility(ESlateVisibility::Visible);
}
