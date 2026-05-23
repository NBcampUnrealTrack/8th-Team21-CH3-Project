// OutGameSettingsWidget.cpp
#include "OutGameUI/Widget/OutGameSettingsWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Game/TeamGameInstance.h"

void UOutGameSettingsWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(mouseSensitivitySlider)) mouseSensitivitySlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleMouseSensitivityChanged);
	if (IsValid(masterVolumeSlider)) masterVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleMasterVolumeChanged);
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
		if (IsValid(mouseSensitivitySlider) == true) mouseSensitivitySlider->SetValue(mouseSensitivity);
		if (IsValid(mouseSensitivityText) == true) mouseSensitivityText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), mouseSensitivity)));
		if (IsValid(masterVolumeSlider) == true) masterVolumeSlider->SetValue(masterVolume);
		if (IsValid(masterVolumeText) == true) masterVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), masterVolume)));
		
		UGameplayStatics::PushSoundMixModifier(this, soundMix);
		
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			soundMix,
			masterSoundClass,
			masterVolume / 100.0f,
			1.0f,
			0.0f,
			true
			);
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
	
	UGameplayStatics::PushSoundMixModifier(this, soundMix);
	
	UGameplayStatics::SetSoundMixClassOverride(
		this,
		soundMix,
		masterSoundClass,
		value / 100.0f,
		1.0f,
		0.0f,
		true
		);
	
	masterVolumeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), value)));
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
