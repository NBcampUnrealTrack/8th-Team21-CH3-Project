// OutGamePlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OutGamePlayerController.generated.h"

class UOutGameRootWidget;

UCLASS()
class TEAM21_CH3_PROJECT_API AOutGamePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OutGame UI")
	TSubclassOf<UOutGameRootWidget> RootWidgetClass;
	
public:
	UFUNCTION()
	void SetViewTargetByTag(FName cameraTag, float blendTime);
	
	UOutGameRootWidget* GetRootWidget() const;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UOutGameRootWidget> RootWidgetInstance;
	
	UFUNCTION()
	void HandleNavigateLeft();
	UFUNCTION()
	void HandleNavigateRight();
	UFUNCTION()
	void HandleEscPressed();
	
	UFUNCTION(Exec)
	void CheatAddKills(int32 Amount);
	UFUNCTION(Exec)
	void CheatAddGold(int32 Amount);
};
