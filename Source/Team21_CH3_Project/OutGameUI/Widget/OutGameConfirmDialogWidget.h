// OutGameConfirmDialogWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameConfirmDialogWidget.generated.h"

enum class EQuitConfirmState : uint8
{
	Closed,
	Opening,
	Opened,
	Closing
};

class UButton;
class UWidgetAnimation;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameConfirmDialogWidget : public UOutGameWidgetBase{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void ToggleQuitConfirm();
	UFUNCTION()
	void ShowQuitConfirm();
	UFUNCTION()
	void HideQuitConfirm();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> confirmButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> cancelButton;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeInAnim;

	UFUNCTION()
	void HandleConfirmClicked();
	UFUNCTION()
	void HandleCancelClicked();
	UFUNCTION()
	void HandleFadeOutFinished();
	UFUNCTION()
	void HandleFadeInFinished();
	
	bool bIsPlay;
	EQuitConfirmState State;
};
