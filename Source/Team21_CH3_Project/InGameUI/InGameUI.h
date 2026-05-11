// InGameUI.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class TEAM21_CH3_PROJECT_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealth(float CurrentHealth, float MaxHealth);
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	void UpdateMatchInfo(int32 PlayerScore, int32 AIScore, int32 Round);

	void ShowRoundTransitionMessage(const FText& MainMessage, const FText& SubMessage);
	void HideRoundTransitionMessage();
	bool IsRoundTransitionMessageVisible() const;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AIScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* RoundTransitionSubText;
};