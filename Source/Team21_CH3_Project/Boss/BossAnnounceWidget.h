// BossAnnounceWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossAnnounceWidget.generated.h"

class UTextBlock;
class UWdigetAnimation;

UCLASS()
class TEAM21_CH3_PROJECT_API UBossAnnounceWidget : public UUserWidget{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	void PlayAnnounceAnimation();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> announceText;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> announceAnim;
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundBase> announceSound;
	
	UFUNCTION()
	void HandleAnnounceFinished();
};
