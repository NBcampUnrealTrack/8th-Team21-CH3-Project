// OutGameTraitWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "Trait/Data/TraitData.h"
#include "OutGameTraitWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetAnimation;
class UOutGameTraitCardWidget;
class UUniformGridPanel;
class UImage;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameTraitWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void RefreshAllTraitCards();
	
private:
	UPROPERTY(meta = (BindWIdget))
	TObjectPtr<UTextBlock> titleText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> requiredTraitText;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> failedFeedbackAnim;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> descriptionText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> stateText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> traitLevelUpButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> unlockGoldCostText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> traitIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> cardContainer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trait", meta = (AllowPrivateAccess))
	TObjectPtr<UDataTable> traitDataTable;
	UPROPERTY()
	TArray<TObjectPtr<UOutGameTraitCardWidget>> traitCards;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trait", meta = (AllowPrivateAccess))
	TSubclassOf<UOutGameTraitCardWidget> traitCardClass;
	UPROPERTY()
	FName selectedTraitId;

	
	UFUNCTION()
	void CreateTraitCards();
	UFUNCTION()
	void RefreshSelectedTraitDetail(FName traitId);
	UFUNCTION()
	void UpdateGoldUI();
	
	
	UFUNCTION()
	void PlayFailedFeedbackAnimation();
	UFUNCTION()
	void HandleTraitCardClicked(FName inTraitId);
	UFUNCTION()
	void HandleFailedFeedBackFinished();
	UFUNCTION()
	void HandleTraitLevelUpClicked();
	
	bool bIsPlaying = false;
};
