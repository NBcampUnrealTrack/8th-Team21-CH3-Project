//ResultUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultUI.generated.h"

class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API UResultUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetResult(bool bPlayerWon);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ResultText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SubText;
};