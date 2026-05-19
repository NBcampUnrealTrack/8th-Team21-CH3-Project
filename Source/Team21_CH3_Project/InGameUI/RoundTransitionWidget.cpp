// RoundTransitionWidget.cpp

#include "RoundTransitionWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void URoundTransitionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// WBP_RoundResult가 생성되었을 때 Fade In 애니메이션을 자동 재생한다.
	if (RoundResultFadeInAnim)
	{
		StopAnimation(RoundResultFadeInAnim);
		PlayAnimation(RoundResultFadeInAnim);
	}
}

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

	// 이미 생성되어 있던 RoundResultWidget을 재사용하는 경우에도
	// 메시지가 갱신될 때 Fade In 애니메이션을 다시 재생한다.
	if (RoundResultFadeInAnim)
	{
		StopAnimation(RoundResultFadeInAnim);
		PlayAnimation(RoundResultFadeInAnim);
	}
}