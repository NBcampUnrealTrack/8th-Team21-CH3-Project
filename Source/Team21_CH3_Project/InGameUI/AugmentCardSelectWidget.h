// AugmentCardSelectWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI/AugmentCardWidget.h"
#include "Data/AugmentationDataTable.h"
#include "AugmentCardSelectWidget.generated.h"

class UAugmentCardWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAugmentSelectedSignature, FAugmentResult, SelectedCardData);

UCLASS()
class TEAM21_CH3_PROJECT_API UAugmentCardSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Augment")
	FAugmentSelectedSignature OnAugmentSelected;

	UFUNCTION(BlueprintCallable)
	void OnDataReceived(const TArray<FAugmentResult>& finalOptions);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UAugmentCardWidget* CardChoice_1;

	UPROPERTY(meta = (BindWidget))
	UAugmentCardWidget* CardChoice_2;

	UPROPERTY(meta = (BindWidget))
	UAugmentCardWidget* CardChoice_3;

private:
	UFUNCTION()
	void HandleCardSelected(FAugmentResult SelectedCardData);
};