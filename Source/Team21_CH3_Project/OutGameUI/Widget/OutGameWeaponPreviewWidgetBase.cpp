// OutGameWeaponPreviewWidgetBase.cpp
#include "OutGameUI/Widget/OutGameWeaponPreviewWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/Preview/OutGameWeaponPreviewManager.h"

bool UOutGameWeaponPreviewWidgetBase::EnterWeaponPreview(){
	return ShowPreviewWeaponByIndex(0);
}

bool UOutGameWeaponPreviewWidgetBase::ShowPreviewWeaponByIndex(int32 weaponIndex){
	AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance();
	if (IsValid(previewManager) == false) return false;

	if (previewManager->ShowWeaponByIndex(weaponIndex) == false) return false;

	currentWeaponData = previewManager->GetCurrentWeaponData();
	if (currentWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("currentWeaponData not found"));
		return false;
	}

	return true;
}

bool UOutGameWeaponPreviewWidgetBase::ShowNextPreviewWeapon(){
	AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance();
	if (IsValid(previewManager) == false) return false;

	if (previewManager->ShowNextWeapon() == false) return false;

	currentWeaponData = previewManager->GetCurrentWeaponData();
	return currentWeaponData != nullptr;
}

bool UOutGameWeaponPreviewWidgetBase::ShowPreviousPreviewWeapon(){
	AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance();
	if (IsValid(previewManager) == false) return false;

	if (previewManager->ShowPreviousWeapon() == false) return false;

	currentWeaponData = previewManager->GetCurrentWeaponData();
	return currentWeaponData != nullptr;
}

void UOutGameWeaponPreviewWidgetBase::ClearWeaponPreview(){
	AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance();
	if (IsValid(previewManager) == false) return;

	previewManager->ClearPreviewWeapon();
	currentWeaponData = nullptr;
}

const FOutGameWeaponPreviewData* UOutGameWeaponPreviewWidgetBase::GetCurrentWeaponData() const{
	return currentWeaponData;
}

AOutGameWeaponPreviewManager* UOutGameWeaponPreviewWidgetBase::GetWeaponPreviewManagerInstance() const{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(this, previewManagerTag, foundActors);

	if (foundActors.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PreviewManager not found by tag: %s"), *previewManagerTag.ToString());
		return nullptr;
	}

	for (AActor* actor : foundActors)
	{
		AOutGameWeaponPreviewManager* previewManager = Cast<AOutGameWeaponPreviewManager>(actor);
		if (IsValid(previewManager) == true)
		{
			return previewManager;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("WeaponPreviewManager actor not found by tag: %s"), *previewManagerTag.ToString());
	return nullptr;
}
