// AugmentCardSelectWidget.cpp
#include "InGameUI/AugmentCardSelectWidget.h"
#include "InGameUI/AugmentCardWidget.h"
#include "Animation/WidgetAnimation.h"

void UAugmentCardSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(CardChoice_1) == true)
	{
		CardChoice_1->OnAugmentCardClicked.RemoveDynamic(this, &UAugmentCardSelectWidget::HandleFirstCardSelected);
		CardChoice_1->OnAugmentCardClicked.AddDynamic(this, &UAugmentCardSelectWidget::HandleFirstCardSelected);
	}

	if (IsValid(CardChoice_2) == true)
	{
		CardChoice_2->OnAugmentCardClicked.RemoveDynamic(this, &UAugmentCardSelectWidget::HandleSecondCardSelected);
		CardChoice_2->OnAugmentCardClicked.AddDynamic(this, &UAugmentCardSelectWidget::HandleSecondCardSelected);
	}

	if (IsValid(CardChoice_3) == true)
	{
		CardChoice_3->OnAugmentCardClicked.RemoveDynamic(this, &UAugmentCardSelectWidget::HandleThirdCardSelected);
		CardChoice_3->OnAugmentCardClicked.AddDynamic(this, &UAugmentCardSelectWidget::HandleThirdCardSelected);
	}
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	FWidgetAnimationDynamicEvent fadeInFinishedEvent;
	fadeInFinishedEvent.BindDynamic(this, &ThisClass::HandleFadeInFinished);
	if (IsValid(fadeInAnim) == true) BindToAnimationFinished(fadeInAnim, fadeInFinishedEvent);
	if (IsValid(fadeInAnim) == true) PlayAnimation(fadeInAnim);
	
	
	FWidgetAnimationDynamicEvent selectedFinishedEvent;
	selectedFinishedEvent.BindDynamic(this, &ThisClass::HandleSelectedAnimFinished);
	if (IsValid(firstCardSelectedAnim) == true) BindToAnimationFinished(firstCardSelectedAnim, selectedFinishedEvent);
	if (IsValid(secondCardSelectedAnim) == true) BindToAnimationFinished(secondCardSelectedAnim, selectedFinishedEvent);
	if (IsValid(thirdCardSelectedAnim) == true) BindToAnimationFinished(thirdCardSelectedAnim, selectedFinishedEvent);
	
}

void UAugmentCardSelectWidget::OnDataReceived(const TArray<FAugmentResult>& finalOptions)
{
	if (CardChoice_1 && finalOptions.IsValidIndex(0))
	{
		CardChoice_1->SetAugmentResult(finalOptions[0]);
	}

	if (CardChoice_2 && finalOptions.IsValidIndex(1))
	{
		CardChoice_2->SetAugmentResult(finalOptions[1]);
	}

	if (CardChoice_3 && finalOptions.IsValidIndex(2))
	{
		CardChoice_3->SetAugmentResult(finalOptions[2]);
	}
}

void UAugmentCardSelectWidget::HandleFadeInFinished(){
	SetVisibility(ESlateVisibility::Visible);
}

void UAugmentCardSelectWidget::HandleFirstCardSelected(FAugmentResult SelectedCardData)
{
	StartCardSelected(SelectedCardData, firstCardSelectedAnim);
}

void UAugmentCardSelectWidget::HandleSecondCardSelected(FAugmentResult SelectedCardData){
	StartCardSelected(SelectedCardData, secondCardSelectedAnim);
}

void UAugmentCardSelectWidget::HandleThirdCardSelected(FAugmentResult SelectedCardData){
	StartCardSelected(SelectedCardData, thirdCardSelectedAnim);
}

void UAugmentCardSelectWidget::StartCardSelected(FAugmentResult selectedCardData, UWidgetAnimation* selectedAnim){
	if (bIsSelecting) return;
	
	bIsSelecting = true;
	pendingSelectedCardData = selectedCardData;
	
	if (IsValid(selectedAnim) == false) return;
	
	PlayAnimation(selectedAnim);
}

void UAugmentCardSelectWidget::HandleSelectedAnimFinished(){
	OnAugmentSelected.Broadcast(pendingSelectedCardData);
}