#include "OutGameUI/Data/OutGameUISoundData.h"

#include "Sound/SoundBase.h"

USoundBase* UOutGameUISoundData::GetSound(EOutGameUISoundType soundType) const{
	const TObjectPtr<USoundBase>* foundSound = UISoundMap.Find(soundType);
	if (foundSound == nullptr)
	{
		return nullptr;
	}

	return foundSound->Get();
}
