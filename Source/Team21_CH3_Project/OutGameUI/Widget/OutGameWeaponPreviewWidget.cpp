// OutGameWeaponPreviewWidget.cpp
#include "OutGameWeaponPreviewWidget.h"
#include "OutGameUI/Preview/OutGameWeaponPreviewManager.h"

void UOutGameWeaponPreviewWidget::NativeOnInitialized(){
	bIsDraggingPreview = false;
}

FReply UOutGameWeaponPreviewWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, 
	const FPointerEvent& InMouseEvent){
	
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton) return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	bIsDraggingPreview = true;
	
	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UOutGameWeaponPreviewWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry, 
	const FPointerEvent& InMouseEvent){
	
	return FReply::Handled();
}

FReply UOutGameWeaponPreviewWidget::NativeOnMouseMove(
	const FGeometry& InGeometry, 
	const FPointerEvent& InMouseEvent){
	
	if (bIsDraggingPreview == false) return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	const FVector2D mouseDelta = InMouseEvent.GetCursorDelta();
	
	//if (AOutGameWeaponPreviewManager* previewManager = Get)
	
	return FReply::Handled();
}
