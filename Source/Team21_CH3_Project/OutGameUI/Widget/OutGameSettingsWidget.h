// OutGameSettingsWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameSettingsWidget.generated.h"

class UButton;
class USlider;
class UComboBoxString;
class USoundMix;
class USoundClass;
class UTextBlock;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameSettingsWidget : public UOutGameWidgetBase{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void UpdateSettings();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> mouseSensitivitySlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> mouseSensitivityText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> graphicsQualityComboBox;
	UPROPERTY()
	float pendingGraphicsQuality;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> applyButton;

	UFUNCTION()
	void HandleMouseSensitivityChanged(float value);
	UFUNCTION()
	void HandleGraphicsQualityChanged(FString selectedItem, ESelectInfo::Type selectionType);
	UFUNCTION()
	void HandleApplyClicked();

	
#pragma region sound

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> masterVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> masterVolumeText;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundClass> masterSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundMix> soundMix;

	UFUNCTION()
	void HandleMasterVolumeChanged(float value);

#pragma endregion
};
