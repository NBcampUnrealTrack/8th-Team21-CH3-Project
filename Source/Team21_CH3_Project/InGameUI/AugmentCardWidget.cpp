// AugmentCardWidget.cpp

#include "InGameUI/AugmentCardWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

void UAugmentCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Select)
	{
		Button_Select->OnClicked.RemoveDynamic(this, &UAugmentCardWidget::HandleSelectButtonClicked);
		Button_Select->OnClicked.AddDynamic(this, &UAugmentCardWidget::HandleSelectButtonClicked);
	}

	SetAugmentCardData(CurrentCardData);
}

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

const FAugmentCardData& UAugmentCardWidget::GetAugmentCardData() const
{
	return CurrentCardData;
}

void UAugmentCardWidget::HandleSelectButtonClicked()
{
	OnAugmentCardClicked.Broadcast(CurrentCardData);
}