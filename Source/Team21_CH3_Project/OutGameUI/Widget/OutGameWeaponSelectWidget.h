// OutGameWeaponSelectWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameUI/Data/OutGameWeaponPreviewData.h"
#include "OutGameWeaponSelectWidget.generated.h"

class UButton;
class UTextBlock;
class AOutGameWeaponPreviewManager;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameWeaponSelectWidget : public UOutGameWidgetBase{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void EnterWeaponSelect();
	UFUNCTION()
	void NavigateWeapon(int32 direction);
	UFUNCTION()
	void RequestBack();
	
private:
	UFUNCTION()
	void HandleNextClicked();
	UFUNCTION()
	void HandlePreviousClicked();
	UFUNCTION()
	void HandleApplyClicked();
	UFUNCTION()
	void HandleBackClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> weaponNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> weaponDamageText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ammoCapacityText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> fireRateText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> weaponRangeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> previousWeaponButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> nextWeaponButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> applyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> backButton;
	
	UFUNCTION()
	void UpdateNextWeaponData(bool bIsNext);
	UFUNCTION()
	void ClearWeaponPreview();
	UFUNCTION()
	void SetWeaponInfo();
	UFUNCTION()
	AOutGameWeaponPreviewManager* GetWeaponPreviewManagerInstance();
	
	void UnlockWeaponCameraMove();
	
	const FOutGameWeaponPreviewData* currentWeaponData;
	FTimerHandle weaponCameraMoveTimerHandle;
	bool bIsWeaponCameraMoving;
};
