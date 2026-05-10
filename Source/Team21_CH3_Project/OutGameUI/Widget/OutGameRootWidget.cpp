// OutGameRootWidget.cpp
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameUI/Widget/OutGameTransitionWidget.h"
#include "OutGameUI/Widget/UOutGameCommonHeaderWidget.h"
#include "OutGameUI/Widget/OutGameQuitConfirmWidget.h"
#include "OutGameUI/Widget/OutGameWeaponSelectWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"

void UOutGameRootWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(TransitionWidget) == true) TransitionWidget->OnFadeOutFinished.AddDynamic(this, &ThisClass::HandleTransitionFadeOutFinished);
	if (IsValid(TransitionWidget) == true) TransitionWidget->OnFadeInFinished.AddDynamic(this, &ThisClass::HandleTransitionFadeInFinished);

	currentWidgetType = EOutGameWidgetType::MainMenu;
	selectedMapLevel = EMapLevel::Easy;
	bIsTransitionPlaying = false;
}

void UOutGameRootWidget::ShowWidget(EOutGameWidgetType widgetType)
{
	currentWidgetType = widgetType;
	if (IsValid(ScreenSwitcher) == true)
	{
		ScreenSwitcher->SetActiveWidgetIndex(int32(widgetType));
		
		if (widgetType == EOutGameWidgetType::WeaponSelect)
		{
			if (UOutGameWeaponSelectWidget* weaponSelectWidget = Cast<UOutGameWeaponSelectWidget>(ScreenSwitcher->GetActiveWidget()))
			{
				weaponSelectWidget->EnterWeaponSelect();
			}
		}
	}
}

void UOutGameRootWidget::ShowSelectTransition(){
	if (IsValid(TransitionWidget) == true) TransitionWidget->PlaySelectTransition();
}

void UOutGameRootWidget::ShowTransition(TFunction<void()> action){
	if (bIsTransitionPlaying == true)
	{
		return;
	}

	bIsTransitionPlaying = true;
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

void UOutGameRootWidget::OpenSelectedLevel(){
	switch (selectedMapLevel)
	{
	case EMapLevel::Easy:
		UGameplayStatics::OpenLevel(this, TEXT("EasyMap"));
		break;
	case EMapLevel::Normal:
		UGameplayStatics::OpenLevel(this, TEXT("NormalMap"));
		break;
	case EMapLevel::Hard:
		UGameplayStatics::OpenLevel(this, TEXT("HardMap"));
		break;
	default:
		break;
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

void UOutGameRootWidget::HandleTransitionFadeInFinished(){
	bIsTransitionPlaying = false;
}

void UOutGameRootWidget::HandleNavigateHorizontal(int32 direction){
	if (currentWidgetType != EOutGameWidgetType::WeaponSelect) return;
	if (IsValid(ScreenSwitcher) == false) return;
	
	if (UOutGameWeaponSelectWidget* weaponSelectWidget = Cast<UOutGameWeaponSelectWidget>(ScreenSwitcher->GetActiveWidget()))
	{
		weaponSelectWidget->NavigateWeapon(direction);
	}
}

void UOutGameRootWidget::HandleBackRequested(){
	if (bIsTransitionPlaying == true) return;
	
	if (currentWidgetType == EOutGameWidgetType::MainMenu)
	{
		if (IsValid(quitConfirmWidget) == true) quitConfirmWidget->ToggleQuitConfirm();
		
		return;
	}
	
	if (currentWidgetType == EOutGameWidgetType::WeaponSelect)
	{
		if (UOutGameWeaponSelectWidget* weaponSelectWidget = Cast<UOutGameWeaponSelectWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			weaponSelectWidget->RequestBack();
		}
		return;
	}
	
	ShowWidget(EOutGameWidgetType::MainMenu);
	SetHeaderVisible(true);
		
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		PC->SetViewTargetByTag("LobbyCamera", 0.0f);
	}
}

EMapLevel UOutGameRootWidget::GetSelectedLevel(){
	return selectedMapLevel;
}

void UOutGameRootWidget::SetSelectedLevel(EMapLevel level){
	selectedMapLevel = level;
}




