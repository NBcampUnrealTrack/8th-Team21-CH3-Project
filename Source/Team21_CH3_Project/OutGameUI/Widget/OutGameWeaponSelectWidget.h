// OutGameWeaponSelectWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWeaponPreviewWidgetBase.h"
#include "OutGameWeaponSelectWidget.generated.h"

class UButton;
class UTextBlock;
class UProgressBar;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameWeaponSelectWidget : public UOutGameWeaponPreviewWidgetBase{
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
	TObjectPtr<UProgressBar> damageBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ammoBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> fireRateBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> weaponRangeBar;
	
	UFUNCTION()
	void UpdateNextWeaponData(bool bIsNext);
	UFUNCTION()
	void UpdateWeaponStatBars(const FOutGameWeaponPreviewData& weaponData);
	UFUNCTION()
	void SetWeaponInfo();
	
	void UnlockWeaponCameraMove();
	
	FTimerHandle weaponCameraMoveTimerHandle;
	bool bIsWeaponCameraMoving;
};
