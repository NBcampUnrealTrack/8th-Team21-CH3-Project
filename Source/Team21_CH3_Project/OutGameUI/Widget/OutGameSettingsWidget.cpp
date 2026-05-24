// OutGameSettingsWidget.cpp
#include "OutGameUI/Widget/OutGameSettingsWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "Game/TeamGameInstance.h"

void UOutGameSettingsWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(mouseSensitivitySlider)) mouseSensitivitySlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleMouseSensitivityChanged);
	if (IsValid(masterVolumeSlider)) masterVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleMasterVolumeChanged);
	if (IsValid(bgmVolumeSlider)) bgmVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleBGMVolumeChanged);
	if (IsValid(sfxVolumeSlider)) sfxVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleSFXVolumeChanged);
	if (IsValid(uiVolumeSlider)) uiVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleUIVolumeChanged);
	if (IsValid(graphicsQualityComboBox)) graphicsQualityComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::HandleGraphicsQualityChanged);
	if (IsValid(applyButton)) applyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleApplyClicked);
	
	pendingGraphicsQuality = 2;
	
	UpdateSettings();
}

void UOutGameSettingsWidget::UpdateSettings(){
	if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
	{
		const float mouseSensitivity = GI->GetMouseSensitivity();
		const float masterVolume = GI->GetMasterVolume();
		const float bgmVolume = GI->GetBGMVolume();
		const float sfxVolume = GI->GetSFXVolume();
		const float uiVolume = GI->GetUIVolume();

		if (IsValid(mouseSensitivitySlider) == true) mouseSensitivitySlider->SetValue(mouseSensitivity);
		if (IsValid(mouseSensitivityText) == true) mouseSensitivityText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), mouseSensitivity)));
		if (IsValid(masterVolumeSlider) == true) masterVolumeSlider->SetValue(masterVolume);
		if (IsValid(masterVolumeText) == true) masterVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), masterVolume)));
		if (IsValid(bgmVolumeSlider) == true) bgmVolumeSlider->SetValue(bgmVolume);
		if (IsValid(bgmVolumeText) == true) bgmVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), bgmVolume)));
		if (IsValid(sfxVolumeSlider) == true) sfxVolumeSlider->SetValue(sfxVolume);
		if (IsValid(sfxVolumeText) == true) sfxVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), sfxVolume)));
		if (IsValid(uiVolumeSlider) == true) uiVolumeSlider->SetValue(uiVolume);
		if (IsValid(uiVolumeText) == true) uiVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), uiVolume)));

		GI->ApplySoundSettings();
	}
}

void UOutGameSettingsWidget::HandleMouseSensitivityChanged(float value){
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (UTeamGameInstance* TeamGameInstance = Cast<UTeamGameInstance>(GameInstance))
		{
			TeamGameInstance->SetMouseSensitivity(value);
		}
	}
	
	mouseSensitivityText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), value)));
}

void UOutGameSettingsWidget::HandleMasterVolumeChanged(float value){
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (UTeamGameInstance* TeamGameInstance = Cast<UTeamGameInstance>(GameInstance))
		{
			TeamGameInstance->SetMasterVolume(value);
		}
	}
	
	masterVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), value)));
}

void UOutGameSettingsWidget::HandleBGMVolumeChanged(float value){
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (UTeamGameInstance* TeamGameInstance = Cast<UTeamGameInstance>(GameInstance))
		{
			TeamGameInstance->SetBGMVolume(value);
		}
	}

	if (IsValid(bgmVolumeText) == true) bgmVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), value)));
}

void UOutGameSettingsWidget::HandleSFXVolumeChanged(float value){
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (UTeamGameInstance* TeamGameInstance = Cast<UTeamGameInstance>(GameInstance))
		{
			TeamGameInstance->SetSFXVolume(value);
		}
	}

	if (IsValid(sfxVolumeText) == true) sfxVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), value)));
}

void UOutGameSettingsWidget::HandleUIVolumeChanged(float value){
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (UTeamGameInstance* TeamGameInstance = Cast<UTeamGameInstance>(GameInstance))
		{
			TeamGameInstance->SetUIVolume(value);
		}
	}

	if (IsValid(uiVolumeText) == true) uiVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), value)));
}

void UOutGameSettingsWidget::HandleGraphicsQualityChanged(FString selectedItem, ESelectInfo::Type selectionType){
	// TODO:Sound
	
	if (selectedItem == TEXT("Low")) pendingGraphicsQuality = 0;
	else if (selectedItem == TEXT("Medium")) pendingGraphicsQuality = 1;
	else if (selectedItem == TEXT("High")) pendingGraphicsQuality = 2;
	else if (selectedItem == TEXT("Epic")) pendingGraphicsQuality = 3;
	else if (selectedItem == TEXT("Cinematic")) pendingGraphicsQuality = 4;
}

void UOutGameSettingsWidget::HandleApplyClicked(){
	PlayUISound(EOutGameUISoundType::Click);
	
	if (UGameUserSettings* gameUserSettings = GEngine->GetGameUserSettings())
	{
		gameUserSettings->SetOverallScalabilityLevel(pendingGraphicsQuality); // 0 Low, 1 Medium, 2 High, 3Epic, 4Cinematic
		gameUserSettings->ApplySettings(false);
		gameUserSettings->SaveSettings();
	}
}
