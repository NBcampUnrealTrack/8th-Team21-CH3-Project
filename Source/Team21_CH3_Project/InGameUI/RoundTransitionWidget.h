// RoundTransitionWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundTransitionWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class TEAM21_CH3_PROJECT_API URoundTransitionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Round Transition")
	void SetRoundMessage(const FText& MainMessage, const FText& SubMessage);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundMessageText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundTransitionSubText;

	// WBP_RoundResult에 만든 Fade In 애니메이션
	// 애니메이션 이름이 RoundResultFadeInAnim이어야 자동 연결된다.
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* RoundResultFadeInAnim;
};