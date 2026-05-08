// OutGameMainMenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameMainMenuWidget.generated.h"

class UButton;
class UWidgetSwitcher;

UCLASS(Abstract, Blueprintable)
class TEAM21_CH3_PROJECT_API UOutGameMainMenuWidget : public UOutGameWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	void ShowLobby();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ScreenSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PlayButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
	
	UFUNCTION()
	void HandlePlayClicked();
	UFUNCTION()
	void HandleQuitClicked();
};
