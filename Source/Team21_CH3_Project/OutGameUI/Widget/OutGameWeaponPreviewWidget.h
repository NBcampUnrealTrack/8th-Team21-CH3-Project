// OutGameWeaponPreviewWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWeaponPreviewWidgetBase.h"
#include "OutGameWeaponPreviewWidget.generated.h"

class UBorder;
class UButton;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameWeaponPreviewWidget : public UOutGameWeaponPreviewWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	
private:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> previewDragArea;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> firstButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> secondButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> thirdButton;
	
	bool InMouseInsidePreviewArea(const FPointerEvent& InMouseEvent) const;
	
	UFUNCTION()
	void HandleFirstClicked();
	UFUNCTION()
	void HandleSecondClicked();
	UFUNCTION()
	void HandleThirdClicked();
	
	bool bIsDraggingPreview;
	
	FTimerHandle weaponCameraMoveTimerHandle;
	bool bIsWeaponCameraMoving;
};
