#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameUISoundData.generated.h"

class USoundBase;

UCLASS(BlueprintType)
class TEAM21_CH3_PROJECT_API UOutGameUISoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TMap<EOutGameUISoundType, TObjectPtr<USoundBase>> UISoundMap;

	USoundBase* GetSound(EOutGameUISoundType soundType) const;
};
