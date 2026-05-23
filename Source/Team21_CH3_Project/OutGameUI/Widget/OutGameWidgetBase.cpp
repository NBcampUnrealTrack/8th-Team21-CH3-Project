// OutGameWidgetBase.cpp
#include "OutGameUI/Widget/OutGameWidgetBase.h"
#include "OutGameUI/Data/OutGameUISoundData.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "Sound/SoundBase.h"

void UOutGameWidgetBase::NativeOnInitialized(){
	Super::NativeOnInitialized();
}

void UOutGameWidgetBase::PlayUISound(EOutGameUISoundType soundType) const{
	const AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>();
	if (IsValid(PC) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("OutGamePlayerController not found"));
		return;
	}

	UOutGameUISoundData* soundData = PC->GetOutGameUISoundData();
	if (IsValid(soundData) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("OutGameUISoundData not found"));
		return;
	}

	USoundBase* sound = soundData->GetSound(soundType);
	if (IsValid(sound) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("UISound not found: %s"), *UEnum::GetValueAsString(soundType));
		return;
	}

	UGameplayStatics::PlaySound2D(this, sound);
}
