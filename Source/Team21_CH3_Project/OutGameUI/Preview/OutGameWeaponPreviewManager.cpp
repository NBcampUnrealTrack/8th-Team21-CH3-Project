// OutGameWeaponPreviewManager.cpp
#include "OutGameWeaponPreviewManager.h"

AOutGameWeaponPreviewManager::AOutGameWeaponPreviewManager(){
	PrimaryActorTick.bCanEverTick = false;
	currentWeaponIndex = 0;
	currentWeaponActor = nullptr;
	previewSpawnPoint = nullptr;
}

const FOutGameWeaponPreviewData* AOutGameWeaponPreviewManager::GetCurrentWeaponData(){
	if (weaponPreviews.IsValidIndex(currentWeaponIndex) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponPreviewManager not found"));
		return nullptr;
	}
	
	return &weaponPreviews[currentWeaponIndex];
}

bool AOutGameWeaponPreviewManager::ShowWeaponByIndex(int32 weaponIndex){
	if (weaponPreviews.IsValidIndex(weaponIndex) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid weapon index"));
		return false;
	}
	
	const FOutGameWeaponPreviewData& data = weaponPreviews[weaponIndex];
	
	if (IsValid(data.weaponClass) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponClass not found"));
		return false;
	}

	ClearPreviewWeapon();
	currentWeaponIndex = weaponIndex;
	
	FTransform spawnTransform = GetActorTransform();
	previewSpawnPoint = data.previewSpawnPoint;
	
	if (IsValid(previewSpawnPoint) == true) spawnTransform = previewSpawnPoint->GetActorTransform();
	
	currentWeaponActor = GetWorld()->SpawnActor<AActor>(
		data.weaponClass,
		spawnTransform
	);

	return IsValid(currentWeaponActor);
}

bool AOutGameWeaponPreviewManager::ShowNextWeapon(){
	if (currentWeaponIndex >= weaponPreviews.Num() - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Weapon"));
		return false;
	}

	return ShowWeaponByIndex(currentWeaponIndex + 1);
}

bool AOutGameWeaponPreviewManager::ShowPreviousWeapon(){
	if (currentWeaponIndex <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("First Weapon"));
		return false;
	}

	return ShowWeaponByIndex(currentWeaponIndex - 1);
}

void AOutGameWeaponPreviewManager::ClearPreviewWeapon(){
	if (currentWeaponActor != nullptr)
	{
		currentWeaponActor->Destroy();
		currentWeaponActor = nullptr;
	}
}

void AOutGameWeaponPreviewManager::SetWeaponIndex(int32 index){
	currentWeaponIndex = index;
}
