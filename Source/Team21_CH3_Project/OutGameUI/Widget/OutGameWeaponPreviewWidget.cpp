// OutGameWeaponPreviewWidget.cpp
#include "OutGameWeaponPreviewWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "OutGameUI/Preview/OutGameWeaponPreviewManager.h"

void UOutGameWeaponPreviewWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(firstButton) == true) firstButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleFirstClicked);
	if (IsValid(secondButton) == true) secondButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSecondClicked);
	if (IsValid(thirdButton) == true) thirdButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleThirdClicked);
	
	bIsDraggingPreview = false;
	previewManagerTag = TEXT("WeaponPreview_PreviewManager");
}

FReply UOutGameWeaponPreviewWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, 
	const FPointerEvent& InMouseEvent){
	
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton
		|| InMouseInsidePreviewArea(InMouseEvent) == false) 
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	bIsDraggingPreview = true;
	
	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UOutGameWeaponPreviewWidget::NativeOnMouseMove(
	const FGeometry& InGeometry, 
	const FPointerEvent& InMouseEvent){
	
	if (bIsDraggingPreview == false) return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	const FVector2D mouseDelta = InMouseEvent.GetCursorDelta();
	
	if (AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance())
	{
		previewManager->RotateCurrentWeapon(mouseDelta.X, mouseDelta.Y);
	}
	
	return FReply::Handled();
}

FReply UOutGameWeaponPreviewWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry, 
	const FPointerEvent& InMouseEvent){
	
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	}
	
	bIsDraggingPreview = false;
	
	return FReply::Handled().ReleaseMouseCapture();
}

bool UOutGameWeaponPreviewWidget::InMouseInsidePreviewArea(const FPointerEvent& InMouseEvent) const{
	if (IsValid(previewDragArea) == false) return false;
	
	const FGeometry previewGeometry = previewDragArea->GetCachedGeometry();
	const FVector2D mouseScreenPosition = InMouseEvent.GetScreenSpacePosition();
	
	return previewGeometry.IsUnderLocation(mouseScreenPosition);
}

void UOutGameWeaponPreviewWidget::HandleFirstClicked(){
	if (ShowPreviewWeaponByIndex(0) == false) return;
}

void UOutGameWeaponPreviewWidget::HandleSecondClicked(){
	if (ShowPreviewWeaponByIndex(1) == false) return;
}

void UOutGameWeaponPreviewWidget::HandleThirdClicked(){
	if (ShowPreviewWeaponByIndex(2) == false) return;
}
