// UOutGameCommonHeaderWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "OutGameWidgetBase.h"
#include "UOutGameCommonHeaderWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API UUOutGameCommonHeaderWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void SetActiveTab(EOutGameWidgetType widgetType);
	UFUNCTION()
	void UpdateCommonUI();
	
private:
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> playButton;
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> weaponsButton;
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> storeButton;
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> settingsButton;
	UPROPERTY(meta =(BindWidget))
	TObjectPtr<UButton> traitButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> playSelectedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> weaponsSelectedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> storeSelectedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> settingsSelectedImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> traitSelectedImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> playerTotalKillCount;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> playerGold;
	
	UFUNCTION()
	void SetSelectedVisible(UImage* image, bool bSelected);
	
	UFUNCTION()
	void HandlePlayClicked();
	UFUNCTION()
	void HandleWeaponsClicked();
	UFUNCTION()
	void HandleStoreButton();
	UFUNCTION()
	void HandleSettingsClicked();
	UFUNCTION()
	void HandleTraitClicked();
	
};
