// OutGameMissionSelect.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameMissionSelectWidget.generated.h"

UENUM()
enum class EMapLevel : uint8{
	Easy = 0 UMETA(DisplayName = "Easy"),
	Normal = 1 UMETA(DisplayName = "Normal"),
	Hard = 2 UMETA(DisplayName = "Hard")
};


class UButton;
class UWidgetSwitcher;
class AOutGameCharacterPreviewManager;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameMissionSelectWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	//UFUNCTION(BlueprintCallable, Category = "OutGame UI")
	//void ShowCharacterSelect();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> ScreenSwitcher;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EasyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NormalButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HardButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton; 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackToMapButton;
	
	void LevelClicked();
	
	UFUNCTION()
	void HandleEasyClicked();
	UFUNCTION()
	void HandleNormalClicked();
	UFUNCTION()
	void HandleHardClicked();
	UFUNCTION()
	void HandleBackClicked();
	
#pragma region CharacterSelect
	
public:
	AOutGameCharacterPreviewManager* GetPreviewManager() const;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> characterSelectButton01;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> characterSelectButton02;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> characterConfirmButton;
	
	EMapLevel selectedMapLevel;
	
	UFUNCTION()
	void HandleCharacter01Clicked();
	UFUNCTION()
	void HandleCharacter02Clicked();
	UFUNCTION()
	void HandleConfirmSelectClicked();
	
	void HandleCharacterSelectClicked(FName characterId);
	
#pragma endregion 

};
