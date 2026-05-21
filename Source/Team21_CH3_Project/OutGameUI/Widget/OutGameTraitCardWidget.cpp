// OutGameTraitCardWidget.cpp
#include "OutGameTraitCardWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UOutGameTraitCardWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(traitCardButton) == true) traitCardButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleTraitCardClicked);
	
	FWidgetAnimationDynamicEvent levelUpAnimFinishedEvent;
	levelUpAnimFinishedEvent.BindDynamic(this, &ThisClass::HandleLevelUpFinished);
	if (IsValid(levelUpAnim) == true) BindToAnimationFinished(levelUpAnim, levelUpAnimFinishedEvent);
}

void UOutGameTraitCardWidget::InitializeCard(FName InTraitId, const FTraitData& InTraitData){
	traitId = InTraitId;
	traitData = InTraitData;
	
	if (IsValid(titleText) == true) titleText->SetText(traitData.traitName);
	if (IsValid(traitIcon) == true) traitIcon->SetBrushFromTexture(traitData.icon);
}

void UOutGameTraitCardWidget::RefreshCard(int32 inCurrentLevel, ETraitCardState inState){
	currentLevel = inCurrentLevel;
	CurrentState = inState;
	
	if (IsValid(lockedIcon) == true) lockedIcon->SetVisibility(ESlateVisibility::Collapsed);
	if (IsValid(traitIcon) == true) traitIcon->SetRenderOpacity(1);
	if (IsValid(titleText) == true) titleText->SetRenderOpacity(1);
	if (IsValid(levelText) == true) levelText->SetRenderOpacity(1);
	if (IsValid(levelText) == true)
	{
		if (currentLevel < maxLevel)
			levelText->SetText(FText::FromString(FString::Printf(TEXT("Lv.%d/5"), currentLevel)));
		else 
			levelText->SetText(FText::FromString(FString::Printf(TEXT("Lv.MAX"))));
	}
	switch (CurrentState)
	{
	case ETraitCardState::Locked:
		{
			if (IsValid(lockedIcon) == true) lockedIcon->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(traitIcon) == true) traitIcon->SetRenderOpacity(0.45);
			if (IsValid(titleText) == true) titleText->SetRenderOpacity(0.45);
			if (IsValid(levelText) == true) levelText->SetRenderOpacity(0.45);
			break;
		}
	case ETraitCardState::MaxLevel:
	case ETraitCardState::NotEnoughGold:
	case ETraitCardState::Upgradeable:
	default:
		break;
	}
}

void UOutGameTraitCardWidget::PlayLevelUpAnimation(){
	if (bIsPlaying) return;
	if (IsValid(levelUpAnim) == false) return;
	
	bIsPlaying = true;
	PlayAnimation(levelUpAnim);
}

void UOutGameTraitCardWidget::HandleTraitCardClicked(){
	OnTraitCardClicked.Broadcast(traitId);
}

void UOutGameTraitCardWidget::HandleLevelUpFinished(){
	bIsPlaying = false;

}

FName UOutGameTraitCardWidget::GetTraitId(){
	return traitId;
}
