// RoundTransitionWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundTransitionWidget.generated.h"

class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API URoundTransitionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Round Transition")
	void SetRoundMessage(const FText& MainMessage, const FText& SubMessage);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundMessageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundTransitionSubText;
};