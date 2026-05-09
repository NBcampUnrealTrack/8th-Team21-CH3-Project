// OutGameQuitConfirmWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameQuitConfirmWidget.generated.h"

class UButton;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameQuitConfirmWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> confirmButton;	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> cancelButton;
	
	UFUNCTION()
	void HandleConfirmClicked();
	UFUNCTION()
	void HandleCancelClicked();
};
