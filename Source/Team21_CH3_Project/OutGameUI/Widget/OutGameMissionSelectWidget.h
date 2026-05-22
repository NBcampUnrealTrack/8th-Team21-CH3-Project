// OutGameMissionSelect.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "OutGameMissionSelectWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameMissionSelectWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EasyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NormalButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HardButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> normalUnlockKillCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> hardUnlockKillCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> playerTotalKillText;
	
	void LevelClicked();
	
	UFUNCTION()
	void HandleEasyClicked();
	UFUNCTION()
	void HandleNormalClicked();
	UFUNCTION()
	void HandleHardClicked();
	UFUNCTION()
	void HandleBackClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnlockKillCount")
	int32 normalUnlockKillCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnlockKillCount")
	int32 hardUnlockKillCount;
	
	EMapLevel selectedMapLevel;
};
