// AugmentCardSelectWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI/AugmentCardWidget.h"
#include "Data/AugmentationDataTable.h"
#include "AugmentCardSelectWidget.generated.h"

class UAugmentCardWidget;
class UWidgetAnimation;

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
	
	UPROPERTY(meta = (BindWidget))
	UAugmentCardWidget* CardChoice_1;
	UPROPERTY(meta = (BindWidget))
	UAugmentCardWidget* CardChoice_2;
	UPROPERTY(meta = (BindWidget))
	UAugmentCardWidget* CardChoice_3;

private:
	UFUNCTION()
	void HandleFadeInFinished();
	UFUNCTION()
	void HandleFirstCardSelected(FAugmentResult SelectedCardData);
	UFUNCTION()
	void HandleSecondCardSelected(FAugmentResult SelectedCardData);
	UFUNCTION()
	void HandleThirdCardSelected(FAugmentResult SelectedCardData);
	UFUNCTION()
	void HandleSelectedAnimFinished();
	
	void StartCardSelected(FAugmentResult selectedCardData, UWidgetAnimation* selectedAnim);
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> fadeInAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> firstCardSelectedAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> secondCardSelectedAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> thirdCardSelectedAnim;
	UPROPERTY()
	FAugmentResult pendingSelectedCardData;

	bool bIsSelecting = false;
};