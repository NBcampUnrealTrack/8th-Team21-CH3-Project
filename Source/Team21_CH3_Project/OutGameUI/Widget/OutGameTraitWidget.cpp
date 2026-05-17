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
		UE_LOG(LogTemp, Warning, TEXT("traitData is nullptr"));
		return;
	}
	
	if (IsValid(titleText) == true) titleText->SetText(traitData->traitName);
	if (IsValid(descriptionText) == true) descriptionText->SetText(traitData->description);
	if (IsValid(unlockGoldCostText) == true) unlockGoldCostText->SetText(FText::FromString(FString::Printf(TEXT("%d G"), traitData->unlockGoldCost)));
	if (IsValid(traitIcon) == true) traitIcon->SetBrushFromTexture(traitData->icon);
	traitIcon->SetVisibility(ESlateVisibility::Visible);
	
	if (IsValid(traitData->))
	
	if (IsValid(requiredTraitText) == true)
	{
		if ()
		
		requiredTraitText->SetText(FText::FromString(FString::Printf(TEXT("%s"), *traitData->RequiredTraitId.ToString())));
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
		if (IsValid(traitLevelUpButton)) traitLevelUpButton->SetVisibility(ESlateVisibility::Collapsed);
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(TEXT("소지 골드가 충분하지않습니다")));
		break;
	case ETraitCardState::MaxLevel:
		if (IsValid(traitLevelUpButton)) traitLevelUpButton->SetVisibility(ESlateVisibility::Collapsed);
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(TEXT("현재 최대 레벨입니다")));
		break;
	case ETraitCardState::Invalid:
		if (IsValid(stateText) == true)stateText->SetText(FText::FromString(FString::Printf(TEXT("%s In valid traitId"), *traitId.ToString())));
	default:
		break;
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
}

void UOutGameTraitWidget::HandleTraitCardClicked(FName inTraitId){
	selectedTraitId = inTraitId;
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
			RefreshSelectedTraitDetail(selectedTraitId);
			RefreshAllTraitCards();
			UpdateGoldUI();
		}
		else
		{
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

void UOutGameTraitWidget::UpdateGoldUI(){
	if (AOutGamePlayerController* PC = Cast<AOutGamePlayerController>(GetOwningPlayer()))
	{
		if (UOutGameRootWidget* RW = PC->GetRootWidget())
		{
			RW->UpdateGoldUI();
		}
	}
}
