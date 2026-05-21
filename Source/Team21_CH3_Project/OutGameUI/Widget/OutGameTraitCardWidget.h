// OutGameTraitCardWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "Trait/Data/TraitData.h"
#include "OutGameTraitCardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTraitCardClicked, FName, traitId);

class UButton;
class UTextBlock;
class UWidgetAnimation;
class UImage;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameTraitCardWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void InitializeCard(FName InTraitId, const FTraitData& InTraitData);
	UFUNCTION()
	void RefreshCard(int32 inCurrentLevel, ETraitCardState inState);
		
	UPROPERTY(BlueprintAssignable)
	FOnTraitCardClicked OnTraitCardClicked;
	
	UFUNCTION()
	FName GetTraitId();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> traitCardButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> titleText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> levelText;
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> levelUpAnim;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> traitIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> lockedIcon;
	
	UFUNCTION()
	void PlayLevelUpAnimation();
	UFUNCTION()
	void HandleTraitCardClicked();
	UFUNCTION()
	void HandleLevelUpFinished();
	
	FTraitData traitData;
	FName traitId;
	int32 currentLevel = 0;
	int32 maxLevel = 5;
	ETraitCardState CurrentState = ETraitCardState::Upgradeable;
	bool bIsPlaying= false;
	
};
