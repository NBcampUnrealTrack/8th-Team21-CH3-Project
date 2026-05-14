// OutGameConfirmDialogWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameConfirmDialogWidget.generated.h"

UENUM()
enum class EQuitConfirmState : uint8
{
	Closed,
	Opening,
	Opened,
	Closing
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmDialogAction);

class UButton;
class UWidgetAnimation;
class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameConfirmDialogWidget : public UOutGameWidgetBase{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	bool IsOpend();
	UFUNCTION()
	void ShowConfirmDialog(const FText& InTitle, const FText& InMessage);
	UFUNCTION()
	void HideConfirmDialog();
	
	UPROPERTY()
	FOnConfirmDialogAction OnConfirmed;
	UPROPERTY()
	FOnConfirmDialogAction OnCanceled;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> titleText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> messageText;
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
	
	FText pendingTitleText;
	FText pendingMessageText;
	bool bIsPlay;
	EQuitConfirmState State;
};
