// AugmentCardWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/AugmentationDataTable.h"
#include "AugmentCardWidget.generated.h"

class UTextBlock;
class UButton;

/*
USTRUCT(BlueprintType)
struct FAugmentCardData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Augment")
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Augment")
	FText CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Augment")
	FText CardEffect;
};
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAugmentCardClickedSignature, FAugmentResult, SelectedCardData);

UCLASS()
class TEAM21_CH3_PROJECT_API UAugmentCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable, Category = "Augment")
	//void SetAugmentCardData(const FAugmentCardData& InCardData);

	UFUNCTION(BlueprintCallable, Category = "Augment")
	void SetAugmentResult(const FAugmentResult& InCardData);

	
	UFUNCTION(BlueprintPure, Category = "Augment")
	const FAugmentResult& GetAugmentCardData() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Augment")
	FAugmentCardClickedSignature OnAugmentCardClicked;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CardNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CardDescriptionText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CardEffectText;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Select;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Augment")
	FAugmentResult CurrentCardData;

private:
	UFUNCTION()
	void HandleSelectButtonClicked();
};