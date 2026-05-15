// OutGameTraitWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameTraitWidget.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameTraitWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
private:
};
