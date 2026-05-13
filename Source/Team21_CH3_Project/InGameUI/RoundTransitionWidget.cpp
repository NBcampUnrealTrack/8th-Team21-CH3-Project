// RoundTransitionWidget.cpp

#include "RoundTransitionWidget.h"
#include "Components/TextBlock.h"

void URoundTransitionWidget::SetRoundMessage(const FText& MainMessage, const FText& SubMessage)
{
	const FText SafeMainMessage = MainMessage.IsEmpty()
		? FText::FromString(TEXT("Next Round"))
		: MainMessage;

	const FText SafeSubMessage = SubMessage.IsEmpty()
		? FText::FromString(TEXT("Get Ready for the Next Round"))
		: SubMessage;

	if (RoundMessageText)
	{
		RoundMessageText->SetText(SafeMainMessage);
		RoundMessageText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (RoundTransitionSubText)
	{
		RoundTransitionSubText->SetText(SafeSubMessage);
		RoundTransitionSubText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}