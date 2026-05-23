// OutGameWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OutGameWidgetBase.generated.h"

class USoundBase;
class UOutGameUISoundData;

UENUM(BlueprintType)
enum class EOutGameUISoundType : uint8{
	Click,
	Back,
	Confirm,
	Cancel,
	Error,
	HideAnim,
	Success,
	Fail,
	OpenTransition,
	HideTransition
};

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UOutGameUISoundData> UISoundData;

	void PlayUISound(EOutGameUISoundType soundType) const;
};
