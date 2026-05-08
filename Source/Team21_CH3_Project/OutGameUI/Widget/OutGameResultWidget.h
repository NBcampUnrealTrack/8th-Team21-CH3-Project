// OutGameResultWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameResultWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameResultWidget : public UOutGameWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> returnToLobbyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> playerTotalKillText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> gameResultText;
	
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UWidgetSwitcher> screenSwitcher;
	
	UFUNCTION()
	void HandleReturnToLobby(); 
};
