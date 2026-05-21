// BossAnnounceWidget.cpp
#include "BossAnnounceWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UBossAnnounceWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(announceText) == true) announceText->SetText(
		FText::FromString(FString::Printf(TEXT("보스가 메테오를 소환합니다!"))));
	
	if (IsValid(announceAnim) == true)
	{
		FWidgetAnimationDynamicEvent announceFinishedEvent;
		announceFinishedEvent.BindDynamic(this, &ThisClass::HandleAnnounceFinished);
		BindToAnimationFinished(announceAnim, announceFinishedEvent);
	}
}

void UBossAnnounceWidget::PlayAnnounceAnimation(){
	if (IsValid(announceSound))
	{
		UGameplayStatics::PlaySound2D(this, announceSound);
	}
	
	if (IsValid(announceAnim) == true)
	{
		PlayAnimation(announceAnim);
	}
}

void UBossAnnounceWidget::HandleAnnounceFinished(){
	
}
