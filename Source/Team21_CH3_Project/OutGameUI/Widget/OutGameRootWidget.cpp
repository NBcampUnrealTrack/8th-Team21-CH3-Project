// OutGameRootWidget.cpp
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "OutGameMainMenuWidget.h"
#include "OutGameMissionSelectWidget.h"
#include "OutGameSettingsWidget.h"
#include "OutGameTraitWidget.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameUI/Widget/OutGameTransitionWidget.h"
#include "OutGameUI/Widget/UOutGameCommonHeaderWidget.h"
#include "OutGameUI/Widget/OutGameConfirmDialogWidget.h"
#include "OutGameUI/Widget/OutGameWeaponSelectWidget.h"
#include "OutGameUI/Widget/OutGameWeaponPreviewWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/Preview/AOutGameCinematicManager.h"

void UOutGameRootWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(TransitionWidget) == true) TransitionWidget->OnFadeOutFinished.AddDynamic(this, &ThisClass::HandleTransitionFadeOutFinished);
	if (IsValid(TransitionWidget) == true) TransitionWidget->OnFadeInFinished.AddDynamic(this, &ThisClass::HandleTransitionFadeInFinished);

	// ConfirmDialogWidget delegate
	if (IsValid(confirmDialogWidget) == true)
	{
		confirmDialogWidget->OnConfirmed.AddUniqueDynamic(this, &ThisClass::HandleConfirmAccepted);
		confirmDialogWidget->OnCanceled.AddUniqueDynamic(this, &ThisClass::HandleConfirmCanceled);
	}
	
	currentWidgetType = EOutGameWidgetType::MainMenu;
	selectedMapLevel = EMapLevel::Easy;
	pendingConfirmAction = EConfirmAction::None;
	bIsTransitionPlaying = false;
}

void UOutGameRootWidget::ShowWidget(EOutGameWidgetType widgetType)
{
	if (IsValid(ScreenSwitcher) == false) return;
	
	if (UOutGameWeaponPreviewWidgetBase* weaponPreviewWidget =
	Cast<UOutGameWeaponPreviewWidgetBase>(ScreenSwitcher->GetActiveWidget()))
	{
		weaponPreviewWidget->ClearWeaponPreview();
	}
	
	currentWidgetType = widgetType;
	ScreenSwitcher->SetActiveWidgetIndex(int32(widgetType));
	commonHeaderWidget->SetActiveTab(widgetType); // commonHeaderWidget 
	commonHeaderWidget->UpdateCommonUI();
	
	if (widgetType == EOutGameWidgetType::WeaponSelect)
	{
		if (UOutGameWeaponSelectWidget* weaponSelectWidget = Cast<UOutGameWeaponSelectWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			weaponSelectWidget->EnterWeaponSelect();
		}
	}
	if (widgetType == EOutGameWidgetType::WeaponPreview)
	{
		if (UOutGameWeaponPreviewWidget* weaponPreviewWidget = Cast<UOutGameWeaponPreviewWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			weaponPreviewWidget->EnterWeaponPreview();
		} 
	}
	if (widgetType == EOutGameWidgetType::Settings)
	{
		if (UOutGameSettingsWidget* settingsWidget = Cast<UOutGameSettingsWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			settingsWidget->UpdateSettings();
		}
	}
	if (widgetType == EOutGameWidgetType::MissionSelect)
	{
		if (UOutGameMissionSelectWidget* missionSelectWidget = Cast<UOutGameMissionSelectWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			missionSelectWidget->PlayFadeInAnimation();
		}
	}
	
}

void UOutGameRootWidget::ShowLobby(){
	if (IsValid(ScreenSwitcher) == true)
	{
		if (UOutGameMainMenuWidget* MainMenuInstance = Cast<UOutGameMainMenuWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			
			MainMenuInstance->ShowLobby();
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
		commonHeaderWidget->UpdateCommonUI();
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

void UOutGameRootWidget::PlayResultCinematic(bool bIsWin){
	AAOutGameCinematicManager* cinematicManager = GetCinematicManager();
	if (IsValid(cinematicManager) == false)
	{
		ShowWidget(EOutGameWidgetType::Result);
		UE_LOG(LogTemp, Warning, TEXT("CinematicManager not found"));
		return;
	}
	
	cinematicManager->OnCinematicFinished.Clear();
	cinematicManager->OnCinematicFinished.AddDynamic(this, &ThisClass::HandleResultCinematicFinished);
	
	cinematicManager->PlayResultCinematic(bIsWin);
}

void UOutGameRootWidget::UpdateGoldUI(){
	commonHeaderWidget->UpdateCommonUI();
}

void UOutGameRootWidget::UpdateTraitUI(){
	if (UOutGameTraitWidget* traitWidget = Cast<UOutGameTraitWidget>(ScreenSwitcher->GetActiveWidget()))
	{
		traitWidget->RefreshAllTraitCards();
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
		// Quit KEY Disabled MainMenuWidget
		UOutGameMainMenuWidget* mainMenuWidget = Cast<UOutGameMainMenuWidget>(ScreenSwitcher->GetActiveWidget());
		if (IsValid(mainMenuWidget) == false || mainMenuWidget->IsLobby() == false)
		{
			return;
		}
		
		if (IsValid(confirmDialogWidget) == false) return;
		
		if (confirmDialogWidget->IsOpend())
		{
			PlayUISound(EOutGameUISoundType::Back);
			confirmDialogWidget->HideConfirmDialog();
		}
		else ShowQuitConfirm();
		
		return;
	}
	
	if (currentWidgetType == EOutGameWidgetType::WeaponSelect)
	{
		if (UOutGameWeaponSelectWidget* weaponSelectWidget = Cast<UOutGameWeaponSelectWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			PlayUISound(EOutGameUISoundType::Back);
			weaponSelectWidget->RequestBack();
		}
		return;
	}
	
	if (currentWidgetType == EOutGameWidgetType::MissionSelect)
	{
		if (UOutGameMissionSelectWidget* missionSelectWidget = Cast<UOutGameMissionSelectWidget>(ScreenSwitcher->GetActiveWidget()))
		{
			missionSelectWidget->PlayFadeOutAnimation();
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

void UOutGameRootWidget::HandleResultCinematicFinished(){
	UE_LOG(LogTemp, Warning, TEXT("Root HandleResultCinematicFinished"));
	ShowWidget(EOutGameWidgetType::Result);
}

void UOutGameRootWidget::SetSelectedLevel(EMapLevel level){
	selectedMapLevel = level;
}

EMapLevel UOutGameRootWidget::GetSelectedLevel(){
	return selectedMapLevel;
}

AAOutGameCinematicManager* UOutGameRootWidget::GetCinematicManager() const{
	UE_LOG(LogTemp, Warning, TEXT("Root PlayResultCinematic"));

	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(
		this,
		AAOutGameCinematicManager::StaticClass(),
		foundActors);
	
	if (foundActors.Num() <= 0) return nullptr;
	
	return Cast<AAOutGameCinematicManager>(foundActors[0]);
}

#pragma region ConfirmDialog

void UOutGameRootWidget::ShowQuitConfirm(){
	pendingConfirmAction = EConfirmAction::QuitGame;
	
	PlayUISound(EOutGameUISoundType::Confirm);
	
	if (IsValid(confirmDialogWidget) == false) return;
	
	confirmDialogWidget->ShowConfirmDialog(
		FText::FromString(TEXT("Quit Game")),
		FText::FromString(TEXT("게임을 종료하시겠습니까?"))
		);
}

void UOutGameRootWidget::ShowNewGameConfirm(){
	pendingConfirmAction = EConfirmAction::NewGame;
	
	PlayUISound(EOutGameUISoundType::Confirm);
	
	if (IsValid(confirmDialogWidget) == false) return;
	
	confirmDialogWidget->ShowConfirmDialog(
		FText::FromString(TEXT("New Game")),
		FText::FromString(TEXT("저장된 플레이 데이터가 삭제됩니다. \n정말 시작하시겠습니까?"))
	);
}

void UOutGameRootWidget::ShowMissionSelectConfirm(){
	pendingConfirmAction = EConfirmAction::MissionSelect;
	
	PlayUISound(EOutGameUISoundType::Error);
	
	if (IsValid(confirmDialogWidget) == false) return;
	confirmDialogWidget->EnableOkButton();
	confirmDialogWidget->ShowConfirmDialog(
		FText::FromString(TEXT("Map Select")),
		FText::FromString(TEXT("해금하기위한 킬 수가 부족합니다!"))
		);
}

void UOutGameRootWidget::ShowWeaponSelectConfirm(){
	pendingConfirmAction = EConfirmAction::WeaponSelect;
	
	PlayUISound(EOutGameUISoundType::Error);
	
	if (IsValid(confirmDialogWidget) == false) return;
	confirmDialogWidget->EnableOkButton();
	confirmDialogWidget->ShowConfirmDialog(
		FText::FromString(TEXT("Weapon Select")),
		FText::FromString(TEXT("해금하기위한 킬 수가 부족합니다!"))
		);
}

void UOutGameRootWidget::HandleConfirmAccepted(){
	switch (pendingConfirmAction)
	{
	case EConfirmAction::QuitGame:
		if (AOutGamePlayerController* PC = Cast<AOutGamePlayerController>(GetOwningPlayer()))
			UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
		break;
	case EConfirmAction::NewGame:
		if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
			GI->StartNewGame();
		if (IsValid(commonHeaderWidget) == true) commonHeaderWidget->UpdateCommonUI();
		ShowTransition([this]
		{
			ShowLobby();
		});
		break;
	case EConfirmAction::MissionSelect:
		
		break;
	default:
		break;
	}
}

void UOutGameRootWidget::HandleConfirmCanceled(){
	pendingConfirmAction = EConfirmAction::None;
}

#pragma endregion 






