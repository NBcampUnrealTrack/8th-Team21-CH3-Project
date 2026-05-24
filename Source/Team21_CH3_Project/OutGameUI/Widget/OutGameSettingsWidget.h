// OutGameSettingsWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameSettingsWidget.generated.h"

class UButton;
class USlider;
class UComboBoxString;
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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> bgmVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> bgmVolumeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> sfxVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> sfxVolumeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> uiVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> uiVolumeText;

	UFUNCTION()
	void HandleMasterVolumeChanged(float value);
	UFUNCTION()
	void HandleBGMVolumeChanged(float value);
	UFUNCTION()
	void HandleSFXVolumeChanged(float value);
	UFUNCTION()
	void HandleUIVolumeChanged(float value);

#pragma endregion
};
