// InGameQuitWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameQuitWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class UTextBlock;
class UWidgetAnimation;
class UOutGameSettingsWidget;

UENUM(BlueprintType)
enum class EQuitWindowState : uint8{
	Closed,
	Opened
};

UENUM(BlueprintType)
enum class EWidgetState : uint8{
	Quit = 0,
	Setting = 1
};

UCLASS()
class TEAM21_CH3_PROJECT_API UInGameQuitWidget : public UUserWidget{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;	
	
	UFUNCTION()
	void HandleBackRequested();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> quitButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> settingButton;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> popInAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> popOutAnim;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> screenSwitcher;
	
	UFUNCTION()
	void ShowWidget(EWidgetState widget);
	
	UFUNCTION()
	void HandleQuitClicked();
	UFUNCTION()
	void HandleSettingClicked();
	UFUNCTION()
	void HandlePopInFinished();
	UFUNCTION()
	void HandlePopOutFinished();
	
	bool bIsOpening;
	EQuitWindowState currentState;
	EWidgetState currentWidgetState;

};
