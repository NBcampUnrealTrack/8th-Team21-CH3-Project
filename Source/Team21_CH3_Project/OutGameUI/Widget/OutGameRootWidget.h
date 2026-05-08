// OutGameRootWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameRootWidget.generated.h"

class UWidgetSwitcher;
class UOutGameTransitionWidget;
class UUOutGameCommonHeaderWidget;

UENUM(BlueprintType)
enum class EOutGameWidgetType : uint8{
	MainMenu = 0 UMETA(DisplayName = "Main Menu"),
	MissionSelect = 1 UMETA(DisplayName = "Mission Select"),
	Weapons = 2 UMETA(DisplayName = "Weapons"),
	Store = 3 UMETA(DisplayName = "Store"),
	Settings = 4 UMETA(DisplayName = "Settings"),
	Result = 5 UMETA(DisplayName = "Result")
};

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameRootWidget : public UOutGameWidgetBase{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowWidget(EOutGameWidgetType widgetType);
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowSelectTransition();

	void ShowTransition(TFunction<void()> action);
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowTransitionFadeOut();
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowTransitionFadein();
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void SetHeaderVisible(bool bVisible);
	

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ScreenSwitcher;
	
	TFunction<void()> pendingTransitionAction;
	
	UFUNCTION()
	void HandleTransitionFadeOutFinished();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOutGameTransitionWidget> TransitionWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUOutGameCommonHeaderWidget> commonHeaderWidget;
};
