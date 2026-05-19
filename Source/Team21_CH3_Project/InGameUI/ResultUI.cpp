// ResultUI.cpp

#include "ResultUI.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UResultUI::NativeConstruct()
{
	Super::NativeConstruct();

	// WBP_ResultUI가 생성되었을 때 Result UI Fade In 애니메이션 재생
	if (ResultFadeInAnim)
	{
		StopAnimation(ResultFadeInAnim);
		PlayAnimation(ResultFadeInAnim);
	}
}

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

	// Result UI가 이미 생성된 상태에서 SetResult가 호출될 수도 있으므로
	// 결과 문구 세팅 후에도 Fade In 애니메이션을 다시 재생한다.
	if (ResultFadeInAnim)
	{
		StopAnimation(ResultFadeInAnim);
		PlayAnimation(ResultFadeInAnim);
	}
}