// InGameQuitWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameQuitWidget.generated.h"


UCLASS()
class TEAM21_CH3_PROJECT_API UInGameQuitWidget : public UUserWidget{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;	
	
	UFUNCTION()
	void HandleBackRequested();
};
