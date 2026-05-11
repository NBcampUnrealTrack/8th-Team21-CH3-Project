// InGameHUD.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameUI;
class URoundTransitionWidget;

UCLASS()
class TEAM21_CH3_PROJECT_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	void RefreshMatchUI(int32 PlayerScore, int32 AIScore, int32 Round);
	void RefreshHealthUI(float CurrentHealth, float MaxHealth);

	void ShowRoundTransitionUI(const FText& MainMessage, const FText& SubMessage);
	void HideRoundTransitionUI();
	bool IsRoundTransitionUIShowing() const;

protected:
	virtual void BeginPlay() override;

	void RefreshMatchUIFromGameInstance();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInGameUI> InGameUIClass;

	UPROPERTY()
	UInGameUI* InGameUIInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Round Transition", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<URoundTransitionWidget> RoundTransitionWidgetClass;

	UPROPERTY()
	URoundTransitionWidget* RoundTransitionWidgetInstance;
};