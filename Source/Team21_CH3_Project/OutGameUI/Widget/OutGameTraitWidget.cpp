// OutGameTraitWidget.cpp
#include "OutGameTraitWidget.h"
#include "OutGameRootWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Trait/SubSystem/TraitSubsystem.h"
#include "Game/TeamGameInstance.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "OutGameUI/Widget/OutGameTraitCardWidget.h"

void UOutGameTraitWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(traitLevelUpButton) == true) traitLevelUpButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleTraitLevelUpClicked);
	
	FWidgetAnimationDynamicEvent failedFeedbackFinishedEvent;
	failedFeedbackFinishedEvent.BindDynamic(this, &ThisClass::HandleFailedFeedBackFinished);
	if (IsValid(failedFeedbackAnim) == true) BindToAnimationFinished(failedFeedbackAnim, failedFeedbackFinishedEvent);
	
	UE_LOG(LogTemp, Warning, TEXT("NativeOnInitialized Called"));
	
	CreateTraitCards();
	RefreshAllTraitCards();
	
}

void UOutGameTraitWidget::CreateTraitCards(){
	UE_LOG(LogTemp, Warning, TEXT("CreateTraitCards Called"));
	if (IsValid(traitDataTable) == false){
		UE_LOG(LogTemp, Warning, TEXT("CreateTraitCards traitDataTable invalid"));
		return;
	}	

	if (IsValid(traitCardClass) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("traitCardClass invalid"));
		return;
	}

	if (IsValid(cardContainer) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("cardContainer invalid"));
		return;
	}
	
	cardContainer->ClearChildren();
	traitCards.Empty();
	
	const TArray<FName> rowNames = traitDataTable->GetRowNames();
	
	for (const FName& rowName : rowNames)
	{
		const FTraitData* traitData =
			traitDataTable->FindRow<FTraitData>(rowName, TEXT("OutGameTraitWidget CreateTraitCards"));
		
		if (traitData == nullptr) continue;
		
		UOutGameTraitCardWidget* traitCard =
			CreateWidget<UOutGameTraitCardWidget>(this, traitCardClass);
		
		if (IsValid(traitCard) == false) continue;
		
		traitCard->InitializeCard(rowName, *traitData);
		
		const int32 index = traitCards.Num();
		const int32 row = index / 5;
		const int32 column = index % 5;
		
		cardContainer->AddChildToUniformGrid(traitCard, row, column);
		
		traitCards.Add(traitCard);
		traitCard->OnTraitCardClicked.AddDynamic(this, &ThisClass::HandleTraitCardClicked);
	}
}

void UOutGameTraitWidget::RefreshSelectedTraitDetail(FName traitId)
{
	if (traitDataTable == nullptr) return;
	
	const FTraitData* traitData =
	traitDataTable->FindRow<FTraitData>(traitId, TEXT("OutGameTraitWidget HandleTraitCardClicked"));
	
	if (traitData == nullptr){
		UE_LOG(LogTemp, Warning, TEXT("traitDa	ta is nullptr"));
		return;
	}
	
	if (IsValid(titleText) == true) titleText->SetText(traitData->traitName);
	if (IsValid(descriptionText) == true) descriptionText->SetText(traitData->description);
	if (IsValid(unlockGoldCostText) == true) unlockGoldCostText->SetText(FText::FromString(FString::Printf(TEXT("%d G"), traitData->unlockGoldCost)));
	if (IsValid(traitIcon) == true)
	{
		traitIcon->SetBrushFromTexture(traitData->icon);
		traitIcon->SetVisibility(ESlateVisibility::Visible);
	}
	
	if (IsValid(requiredTraitText) == true)
	{
		if (traitData->RequiredTraitId.IsNone() || traitData->RequiredTraitId == FName(TEXT("None")))
		{
			requiredTraitText->SetText(FText::FromString(FString::Printf(TEXT("선행 조건 없음"))));
		}
		else
		{
			const FTraitData* traitDataTemp =
				traitDataTable->FindRow<FTraitData>(traitData->RequiredTraitId, TEXT("traitDataTemp"));

			if (traitDataTemp == nullptr)
			{
				requiredTraitText->SetText(FText::FromString(FString::Printf(TEXT("선행 조건 없음"))));
				UE_LOG(LogTemp, Warning, TEXT("traitDataTemp in valid"));
				return;
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Clicked TraitId: %s"), *traitDataTemp->RequiredTraitId.ToString());
			requiredTraitText->SetText(FText::FromString(FString::Printf(TEXT("%s Lv.MAX"), *traitDataTemp->traitName.ToString())));
		}
	}
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false) return;
	UTraitSubsystem* subsystem = GI->GetSubsystem<UTraitSubsystem>();
	if (IsValid(subsystem) == false) return;
	
	const int32 traitLevel = GI->GetTraitLevel(traitId);
	const ETraitCardState currentState = subsystem->GetTraitCardState(traitId, *traitData);

	if (IsValid(traitLevelUpButton) == true) traitLevelUpButton->SetVisibility(ESlateVisibility::Visible);
	
	// Locked → Opacity down, LevelUpButton disabled
	// MaxLevel → Opacity 1, LevelUpButton enabled / Lv.MAX
	// NotEnoughGold → Opacity 1, LevelUpButton enabled
	// Upgradeable → Opacity 1, LevelUpButton enabled
	switch (currentState)
	{
	case ETraitCardState::Locked:
		if (IsValid(traitLevelUpButton)) traitLevelUpButton->SetVisibility(ESlateVisibility::Collapsed);
		if (IsValid(stateText) == true) stateText->SetText(FText::FromString(TEXT("선행 특성 Lv.5 필요")));
		break;
	case ETraitCardState::Upgradeable:
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(TEXT("Level Up")));
		break;
	case ETraitCardState::NotEnoughGold:
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(TEXT("소지 골드가 충분하지않습니다")));
		break;
	case ETraitCardState::MaxLevel:
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(TEXT("현재 최대 레벨입니다")));
		break;
	case ETraitCardState::Invalid:
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(FString::Printf(TEXT("%s In valid traitId"), *traitId.ToString())));
	default:
		break;
	}
}

void UOutGameTraitWidget::RefreshChangeBonusDetail(FName inTraitId){
	FTraitData* traitData =
		traitDataTable->FindRow<FTraitData>(inTraitId, TEXT("RefreshChangeBonusDetail"));
	
	if (traitData == nullptr) return;
	
	TArray<FTraitEffectLine> effects;
	
	if (!FMath::IsNearlyZero(traitData->weaponDamageBonus))
		effects.Add({TEXT("무기 피해"), traitData->weaponDamageBonus, true});
	if (!FMath::IsNearlyZero(traitData->maxHPBonus))
		effects.Add({TEXT("최대 체력"),traitData->maxHPBonus, false});
	if (!FMath::IsNearlyZero(traitData->moveSpeedBonus))
		effects.Add({TEXT("이동 속도"), traitData->moveSpeedBonus, true});
	if (!FMath::IsNearlyZero(traitData->reloadSpeedBonus))
		effects.Add({TEXT("재장전 속도"), traitData->reloadSpeedBonus, true});
	if (!FMath::IsNearlyZero(traitData->goldGainBonus))
		effects.Add({TEXT("골드 획득"), traitData->goldGainBonus, true});
	
	TArray<UTextBlock*> effectTexts ={
		effectText0,
		effectText1,
		effectText2,
		effectText3
	};
	
	for (int32 i = 0; i < effectTexts.Num(); ++i)
	{
		if (IsValid(effectTexts[i]) == false) continue;
		
		if (!effects.IsValidIndex(i))
		{
			effectTexts[i]->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}
		
		const FTraitEffectLine& effect = effects[i];
		
		const float displayValue = effect.bPercent ? effect.value * 100.0f : effect.value;
		const FString suf = effect.bPercent ? TEXT("%") : TEXT("");
		const FString displayText = effect.bPercent 
		? FString::Printf(TEXT("%s %+.1f%s"), *effect.label, displayValue, *suf) 
		: FString::Printf(TEXT("%s %+.0f%s"), *effect.label, displayValue, *suf);
		
		effectTexts[i]->SetVisibility(ESlateVisibility::Visible);
		effectTexts[i]->SetText(FText::FromString(displayText));
		
		const FLinearColor Color = 
			displayValue >= 0.0f ? FLinearColor::Green : FLinearColor::Red;
		
		effectTexts[i]->SetColorAndOpacity(FSlateColor(Color));
	}
	
}

void UOutGameTraitWidget::RefreshAllTraitCards(){
	for (auto& traitCard : traitCards)
	{
		FName traitId = traitCard->GetTraitId();
		
		FTraitData* traitData =
			traitDataTable->FindRow<FTraitData>(traitId, TEXT("OutGameTraitWidget RefreshAllTraitCards"));
		if (traitData == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("traitData is nullptr"));
			continue;
		}
		
		if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (UTraitSubsystem* subsystem = GI->GetSubsystem<UTraitSubsystem>())
			{
				const ETraitCardState state = subsystem->GetTraitCardState(traitId, *traitData);
				const int32 level = GI->GetTraitLevel(traitId);
				
				traitCard->RefreshCard(level, state);

			}
		}
	}
	
	UpdateTraitBonus();
}

void UOutGameTraitWidget::HandleTraitCardClicked(FName inTraitId){
	PlayUISound(EOutGameUISoundType::Click);
	
	selectedTraitId = inTraitId;
	RefreshChangeBonusDetail(inTraitId);
	RefreshSelectedTraitDetail(inTraitId);
}

void UOutGameTraitWidget::HandleTraitLevelUpClicked(){
	if (traitDataTable == nullptr) return;
	
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false) return;
	
	if (UTraitSubsystem* subsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTraitSubsystem>())
	{
		FTraitData* traitData =
			traitDataTable->FindRow<FTraitData>(selectedTraitId, TEXT("OutGameTraitWidget HandleTraitLevelUpClicked"));
		if (traitData == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("traitData is nullptr"));
			return;
		}
		
		if (subsystem->TryUpgradeTrait(selectedTraitId, *traitData))
		{
			PlayUISound(EOutGameUISoundType::Success);
			RefreshSelectedTraitDetail(selectedTraitId);
			RefreshAllTraitCards();
			UpdateGoldUI();
		}
		else
		{
			PlayUISound(EOutGameUISoundType::Fail);
			PlayFailedFeedbackAnimation();
		}
	}
}

void UOutGameTraitWidget::PlayFailedFeedbackAnimation(){
	if (IsValid(failedFeedbackAnim) == false) return;
	
	if (bIsPlaying) return;
	bIsPlaying = true;
	
	if (IsValid(traitLevelUpButton) == true) traitLevelUpButton->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	PlayAnimation(failedFeedbackAnim);
}

void UOutGameTraitWidget::HandleFailedFeedBackFinished(){
	bIsPlaying = false;
	
	if (IsValid(traitLevelUpButton) == true) traitLevelUpButton->SetVisibility(ESlateVisibility::Visible);
}

void UOutGameTraitWidget::UpdateTraitBonus(){
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GI) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateTraitBonus GameInstance in valid"));
		return;
	}
	UTraitSubsystem* subsystem = GI->GetSubsystem<UTraitSubsystem>();
	if (IsValid(subsystem) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateTraitBonus subsystem in valid"));
		return;
	}
	
	const FPlayerTraitBonus bonus = subsystem->CalculateTotalTraitBonus(traitDataTable);
	
	if (IsValid(weaponDamageBonusText) == true) 
		weaponDamageBonusText->SetText(FText::FromString(FString::Printf(TEXT("%+.0f%%"), bonus.weaponDamageBonus * 100.0f)));
	if (IsValid(maxHPBonusText) == true) 
		maxHPBonusText->SetText(FText::FromString(FString::Printf(TEXT("%+.0f"), bonus.maxHPBonus)));
	if (IsValid(moveSpeedBonusText) == true) 
		moveSpeedBonusText->SetText(FText::FromString(FString::Printf(TEXT("%+.0f%%"), bonus.moveSpeedBonus * 100.0f)));
	if (IsValid(reloadSpeedBonusText) == true) 
		reloadSpeedBonusText->SetText(FText::FromString(FString::Printf(TEXT("%+.0f%%"), bonus.reloadSpeedBonus * 100.0f)));
	if (IsValid(goldGainBonusText) == true) 
		goldGainBonusText->SetText(FText::FromString(FString::Printf(TEXT("%+.0f%%"), bonus.goldGainBonus * 100.0f)));
	
	// GI->SetGoldGainMultiplier(bonus.goldGainBonus);
}

void UOutGameTraitWidget::UpdateGoldUI(){
	if (AOutGamePlayerController* PC = Cast<AOutGamePlayerController>(GetOwningPlayer()))
	{
		if (UOutGameRootWidget* RW = PC->GetRootWidget())
		{
			RW->UpdateGoldUI();
		}
	}
}
