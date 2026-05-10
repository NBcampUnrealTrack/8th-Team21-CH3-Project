// OutGameWeaponPreviewWidgetBase.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameUI/Data/OutGameWeaponPreviewData.h"
#include "OutGameWidgetBase.h"
#include "OutGameWeaponPreviewWidgetBase.generated.h"

class AOutGameWeaponPreviewManager;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameWeaponPreviewWidgetBase : public UOutGameWidgetBase
{
	GENERATED_BODY()

public:
	bool EnterWeaponPreview();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview")
	FName previewManagerTag;

	const FOutGameWeaponPreviewData* currentWeaponData = nullptr;

	bool ShowPreviewWeaponByIndex(int32 weaponIndex);
	bool ShowNextPreviewWeapon();
	bool ShowPreviousPreviewWeapon();
	void ClearWeaponPreview();
	const FOutGameWeaponPreviewData* GetCurrentWeaponData() const;
	AOutGameWeaponPreviewManager* GetWeaponPreviewManagerInstance() const;
};
