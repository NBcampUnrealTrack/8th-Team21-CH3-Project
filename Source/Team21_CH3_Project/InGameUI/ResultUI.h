// ResultUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultUI.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class TEAM21_CH3_PROJECT_API UResultUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetResult(bool bPlayerWon);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ResultText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* SubText;

	// WBP_ResultUI에 만든 Fade In 애니메이션
	// 애니메이션 이름이 ResultFadeInAnim이어야 자동 연결된다.
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* ResultFadeInAnim;
};