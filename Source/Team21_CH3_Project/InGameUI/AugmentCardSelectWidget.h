// AugmentCardSelectWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI/AugmentCardWidget.h"
#include "AugmentCardSelectWidget.generated.h"

class UAugmentCardWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAugmentSelectedSignature, FAugmentCardData, SelectedCardData);

UCLASS()
class TEAM21_CH3_PROJECT_API UAugmentCardSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Augment")
	FAugmentSelectedSignature OnAugmentSelected;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UAugmentCardWidget* CardChoice_1;

	UPROPERTY(meta = (BindWidgetOptional))
	UAugmentCardWidget* CardChoice_2;

	UPROPERTY(meta = (BindWidgetOptional))
	UAugmentCardWidget* CardChoice_3;

private:
	UFUNCTION()
	void HandleCardSelected(FAugmentCardData SelectedCardData);
};