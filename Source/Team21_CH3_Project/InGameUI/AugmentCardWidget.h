// AugmentCardWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AugmentCardWidget.generated.h"

class UTextBlock;
class UButton;

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

UCLASS()
class TEAM21_CH3_PROJECT_API UAugmentCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Augment")
	void SetAugmentCardData(const FAugmentCardData& InCardData);

	UFUNCTION(BlueprintPure, Category = "Augment")
	const FAugmentCardData& GetAugmentCardData() const;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CardNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CardDescriptionText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CardEffectText;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* Button_Select;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Augment")
	FAugmentCardData CurrentCardData;
};