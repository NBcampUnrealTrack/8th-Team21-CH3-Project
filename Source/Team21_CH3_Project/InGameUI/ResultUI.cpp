//Result.cpp

#include "ResultUI.h"
#include "Components/TextBlock.h"

void UResultUI::SetResult(bool bPlayerWon)
{
	if (ResultText)
	{
		ResultText->SetText(
			bPlayerWon
			? FText::FromString(TEXT("VICTORY"))
			: FText::FromString(TEXT("DEFEAT"))
		);
	}

	if (SubText)
	{
		SubText->SetText(
			bPlayerWon
			? FText::FromString(TEXT("All waves cleared. Returning to lobby..."))
			: FText::FromString(TEXT("You have been eliminated. Returning to lobby..."))
		);
	}
}