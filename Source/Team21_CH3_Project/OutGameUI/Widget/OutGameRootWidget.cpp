// OutGameRootWidget.cpp
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Components/WidgetSwitcher.h"
#include "OutGameUI/Widget/OutGameTransitionWidget.h"
#include "OutGameUI/Widget/UOutGameCommonHeaderWidget.h"

void UOutGameRootWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(TransitionWidget) == true) TransitionWidget->OnFadeOutFinished.AddDynamic(this, &ThisClass::HandleTransitionFadeOutFinished);
}

void UOutGameRootWidget::ShowWidget(EOutGameWidgetType widgetType)
{
	if (IsValid(ScreenSwitcher) == true)
	{
		ScreenSwitcher->SetActiveWidgetIndex(int32(widgetType));
	}
}

void UOutGameRootWidget::ShowSelectTransition(){
	if (IsValid(TransitionWidget) == true) TransitionWidget->PlaySelectTransition();
}

void UOutGameRootWidget::ShowTransition(TFunction<void()> action){
	pendingTransitionAction = MoveTemp(action);
	
	if (IsValid(TransitionWidget) == true) TransitionWidget->PlayFadeOut();
}

void UOutGameRootWidget::ShowTransitionFadeOut(){
	if (IsValid(TransitionWidget) == true) TransitionWidget->PlayFadeOut();
}

void UOutGameRootWidget::ShowTransitionFadein(){
	if (IsValid(TransitionWidget) == true) TransitionWidget->PlayFadeIn();
}

void UOutGameRootWidget::SetHeaderVisible(bool bVisible){
	if (IsValid(commonHeaderWidget) == true)
	{
		commonHeaderWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UOutGameRootWidget::HandleTransitionFadeOutFinished(){
	if (pendingTransitionAction)
	{
		pendingTransitionAction();
		pendingTransitionAction = nullptr;
	}
	if (IsValid(TransitionWidget) == true)
	{
		TransitionWidget->PlayFadeIn();
	}
}



