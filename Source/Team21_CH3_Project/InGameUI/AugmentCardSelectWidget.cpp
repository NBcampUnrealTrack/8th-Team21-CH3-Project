// AugmentCardSelectWidget.cpp
#include "InGameUI/AugmentCardSelectWidget.h"
#include "InGameUI/AugmentCardWidget.h"

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

void UAugmentCardSelectWidget::OnDataReceived(const TArray<FAugmentResult>& finalOptions)
{
	if (CardChoice_1 && finalOptions.IsValidIndex(0))
	{
		CardChoice_1->SetAugmentResult(finalOptions[0]);
	}

	if (CardChoice_2 && finalOptions.IsValidIndex(1))
	{
		CardChoice_2->SetAugmentResult(finalOptions[1]);
	}

	if (CardChoice_3 && finalOptions.IsValidIndex(2))
	{
		CardChoice_3->SetAugmentResult(finalOptions[2]);
	}
}


void UAugmentCardSelectWidget::HandleCardSelected(FAugmentResult SelectedCardData)
{
	OnAugmentSelected.Broadcast(SelectedCardData);
}