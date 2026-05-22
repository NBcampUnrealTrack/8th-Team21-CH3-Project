// OutGameRootWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameRootWidget.generated.h"

class UWidgetSwitcher;
class UOutGameConfirmDialogWidget;
class UOutGameTransitionWidget;
class UUOutGameCommonHeaderWidget;
class AAOutGameCinematicManager;

UENUM(BlueprintType)
enum class EOutGameWidgetType : uint8{
	MainMenu = 0 UMETA(DisplayName = "Main Menu"),
	MissionSelect = 1 UMETA(DisplayName = "Mission Select"),
	WeaponPreview = 2 UMETA(DisplayName = "WeaponPreview"),
	Store = 3 UMETA(DisplayName = "Store"),
	WeaponSelect = 4 UMETA(DisplayName = "WeaponSelect"),
	Settings = 5 UMETA(DisplayName = "Settings"),
	Trait = 6 UMETA(DisplayName = "Trait"),
	Result = 7 UMETA(DisplayName = "Result"),
	None = 8
};

UENUM(BlueprintType)
enum class EMapLevel : uint8{
	Easy = 0 UMETA(DisplayName = "Easy"),
	Normal = 1 UMETA(DisplayName = "Normal"),
	Hard = 2 UMETA(DisplayName = "Hard")
};

UENUM(BlueprintType)
enum class EConfirmAction : uint8{
	None = 0 UMETA(DisplayName = "None"),
	QuitGame = 1 UMETA(DisplayName = "QuitGame"),
	NewGame = 2 UMETA(DisplayName = "NewGame"),
	MissionSelect = 3 UMETA(DisplayName = "MissionSelect"),
	WeaponSelect = 4 UMETA(DisplayName = "WeaponSelect")
};

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameRootWidget : public UOutGameWidgetBase{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowWidget(EOutGameWidgetType widgetType);
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowLobby();
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowSelectTransition();
	
	void ShowTransition(TFunction<void()> action);
	
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowTransitionFadeOut();
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowTransitionFadein();
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void SetHeaderVisible(bool bVisible);
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void OpenSelectedLevel();
	UFUNCTION()
	void PlayResultCinematic(bool bIsWin);
	UFUNCTION()
	void UpdateGoldUI();
	UFUNCTION()
	void UpdateTraitUI();
	
	UFUNCTION()
	void HandleNavigateHorizontal(int32 direction);
	UFUNCTION()
	void HandleBackRequested();
	UFUNCTION()
	void HandleResultCinematicFinished();
	
	UFUNCTION()
	void SetSelectedLevel(EMapLevel level);
	UFUNCTION()
	EMapLevel GetSelectedLevel();
	
	AAOutGameCinematicManager* GetCinematicManager() const;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ScreenSwitcher;
	
	TFunction<void()> pendingTransitionAction;
	
	UFUNCTION()
	void HandleTransitionFadeOutFinished();
	UFUNCTION()
	void HandleTransitionFadeInFinished();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOutGameTransitionWidget> TransitionWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUOutGameCommonHeaderWidget> commonHeaderWidget;
	
	EOutGameWidgetType currentWidgetType;
	EMapLevel selectedMapLevel;
	bool bIsTransitionPlaying;
	
#pragma region ConfirmDialog
	
public:
	void ShowQuitConfirm();
	void ShowNewGameConfirm();	
	void ShowMissionSelectConfirm();
	void ShowWeaponSelectConfirm();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOutGameConfirmDialogWidget> confirmDialogWidget;
	
	EConfirmAction pendingConfirmAction;
	
	UFUNCTION()
	void HandleConfirmAccepted();
	UFUNCTION()
	void HandleConfirmCanceled();

#pragma endregion 
};
