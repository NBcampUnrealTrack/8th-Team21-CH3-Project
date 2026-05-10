// OutGameWeaponPreviewWidget.h
#pragma once

#include "CoreMinimal.h"
#include "OutGameWidgetBase.h"
#include "OutGameWeaponPreviewWidget.generated.h"

class UBorder;

UCLASS()
class TEAM21_CH3_PROJECT_API UOutGameWeaponPreviewWidget : public UOutGameWidgetBase{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	
private:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> previewDragArea;
	
	bool bIsDraggingPreview;
};
