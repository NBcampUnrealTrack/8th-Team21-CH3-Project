// BossAnnounceWidget.cpp
#include "BossAnnounceWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UBossAnnounceWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	

	
	if (IsValid(announceAnim) == true)
	{
		FWidgetAnimationDynamicEvent announceFinishedEvent;
		announceFinishedEvent.BindDynamic(this, &ThisClass::HandleAnnounceFinished);
		BindToAnimationFinished(announceAnim, announceFinishedEvent);
	}
}

void UBossAnnounceWidget::PlayAnnounceAnimation(const FText& InMessage){
	if (IsValid(announceText) == true) announceText->SetText(InMessage);
	
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
	SetVisibility(ESlateVisibility::Collapsed);
}
