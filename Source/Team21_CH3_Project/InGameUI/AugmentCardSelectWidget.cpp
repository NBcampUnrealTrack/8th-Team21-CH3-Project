// AugmentCardSelectWidget.cpp

#include "InGameUI/AugmentCardSelectWidget.h"

void UAugmentCardSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CardChoice_1)
	{
		CardChoice_1->OnAugmentCardClicked.RemoveDynamic(this, &UAugmentCardSelectWidget::HandleCardSelected);
		CardChoice_1->OnAugmentCardClicked.AddDynamic(this, &UAugmentCardSelectWidget::HandleCardSelected);
	}

	if (CardChoice_2)
	{
		CardChoice_2->OnAugmentCardClicked.RemoveDynamic(this, &UAugmentCardSelectWidget::HandleCardSelected);
		CardChoice_2->OnAugmentCardClicked.AddDynamic(this, &UAugmentCardSelectWidget::HandleCardSelected);
	}

	if (CardChoice_3)
	{
		CardChoice_3->OnAugmentCardClicked.RemoveDynamic(this, &UAugmentCardSelectWidget::HandleCardSelected);
		CardChoice_3->OnAugmentCardClicked.AddDynamic(this, &UAugmentCardSelectWidget::HandleCardSelected);
	}
}

void UAugmentCardSelectWidget::HandleCardSelected(FAugmentCardData SelectedCardData)
{
	OnAugmentSelected.Broadcast(SelectedCardData);
}