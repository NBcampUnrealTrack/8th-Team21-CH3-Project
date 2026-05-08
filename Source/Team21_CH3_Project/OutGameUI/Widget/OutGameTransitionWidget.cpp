// OutGameTransitionWidget.cpp
#include "OutGameUI/Widget/OutGameTransitionWidget.h"
#include "Animation/WidgetAnimation.h"

void UOutGameTransitionWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
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
	
	if (IsValid(SelectAnim) == true)
	{
		FWidgetAnimationDynamicEvent selectFinishedEvent;
		selectFinishedEvent.BindDynamic(this, &ThisClass::HandleSelectFinished);
		BindToAnimationFinished(SelectAnim, selectFinishedEvent);
	}
	
}

void UOutGameTransitionWidget::PlayFadeOut(){
	SetVisibility(ESlateVisibility::Visible);
	
	if (IsValid(FadeOutAnim) == true) PlayAnimation(FadeOutAnim);
}

void UOutGameTransitionWidget::PlayFadeIn(){
	SetVisibility(ESlateVisibility::Visible);
	
	if (IsValid(FadeInAnim) == true) PlayAnimation(FadeInAnim);
}

void UOutGameTransitionWidget::HandleFadeOutFinished(){
	OnFadeOutFinished.Broadcast();
}

void UOutGameTransitionWidget::HandleFadeInFinished(){
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOutGameTransitionWidget::HandleSelectFinished(){
	SetVisibility(ESlateVisibility::Collapsed);
}

void UOutGameTransitionWidget::PlaySelectTransition(){
	SetVisibility(ESlateVisibility::Visible);
	
	if (IsValid(SelectAnim) == true) PlayAnimation(SelectAnim);
	else SetVisibility(ESlateVisibility::Collapsed);
}


