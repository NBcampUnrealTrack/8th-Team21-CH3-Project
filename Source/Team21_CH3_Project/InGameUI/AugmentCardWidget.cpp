// AugmentCardWidget.cpp

#include "InGameUI/AugmentCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UAugmentCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Select)
	{
		Button_Select->OnClicked.RemoveDynamic(this, &UAugmentCardWidget::HandleSelectButtonClicked);
		Button_Select->OnClicked.AddDynamic(this, &UAugmentCardWidget::HandleSelectButtonClicked);
	}

	//SetAugmentResult(CurrentCardData);
}

/*
void UAugmentCardWidget::SetAugmentCardData(const FAugmentCardData& InCardData)
{
	CurrentCardData = InCardData;

	if (CardNameText)
	{
		CardNameText->SetText(CurrentCardData.CardName);
	}

	if (CardDescriptionText)
	{
		CardDescriptionText->SetText(CurrentCardData.CardDescription);
	}

	if (CardEffectText)
	{
		CardEffectText->SetText(CurrentCardData.CardEffect);
	}
}
*/

void UAugmentCardWidget::SetAugmentResult(const FAugmentResult& InCardData){
	CurrentCardData = InCardData;
	
	UE_LOG(LogTemp, Warning, TEXT("SetAugmentResult: %s / %s"),
		*CurrentCardData.DisplayTitle,
		*CurrentCardData.Description
	);

	UE_LOG(LogTemp, Warning, TEXT("CardNameText: %s"), IsValid(CardNameText) ? TEXT("Valid") : TEXT("Null"));
	UE_LOG(LogTemp, Warning, TEXT("CardDescriptionText: %s"), IsValid(CardDescriptionText) ? TEXT("Valid") : TEXT("Null"));

	if (CardNameText)
	{
		CardNameText->SetText(FText::FromString(CurrentCardData.DisplayTitle));
	}

	if (CardDescriptionText)
	{
		CardDescriptionText->SetText(FText::FromString(CurrentCardData.Description));
	}
	
	if (IsValid(cardIcon) == true) cardIcon->SetBrushFromTexture(InCardData.icon);
}

const FAugmentResult& UAugmentCardWidget::GetAugmentCardData() const
{
	return CurrentCardData;
}

void UAugmentCardWidget::HandleSelectButtonClicked()
{
	OnAugmentCardClicked.Broadcast(CurrentCardData);
}