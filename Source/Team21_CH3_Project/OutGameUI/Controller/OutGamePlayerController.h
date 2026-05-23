// OutGamePlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OutGamePlayerController.generated.h"

class UOutGameRootWidget;
class UOutGameUISoundData;
class USoundBase;
class UAudioComponent;

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
	UOutGameUISoundData* GetOutGameUISoundData() const;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UOutGameRootWidget> RootWidgetInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOutGameUISoundData> OutGameUISoundData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound|BGM", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> OutGameBGM;
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> OutGameBGMComponent;
	
	UFUNCTION()
	void HandleNavigateLeft();
	UFUNCTION()
	void HandleNavigateRight();
	UFUNCTION()
	void HandleEscPressed();
	
	void PlayOutGameBGM();
	void StopOutGameBGM();
	
	UFUNCTION(Exec)
	void CheatAddKills(int32 Amount);
	UFUNCTION(Exec)
	void CheatAddGold(int32 Amount);
};
