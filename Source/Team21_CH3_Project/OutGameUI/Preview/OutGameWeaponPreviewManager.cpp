// OutGameWeaponPreviewManager.cpp
#include "OutGameWeaponPreviewManager.h"

AOutGameWeaponPreviewManager::AOutGameWeaponPreviewManager(){
	PrimaryActorTick.bCanEverTick = false;
	currentWeaponIndex = 0;
	currentWeaponActor = nullptr;
	previewSpawnPoint = nullptr;
	previewWeaponScale = 1.0f;
}

const FOutGameWeaponPreviewData* AOutGameWeaponPreviewManager::GetCurrentWeaponData(){
	if (weaponPreviews.IsValidIndex(currentWeaponIndex) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponPreviewManager not found"));
		return nullptr;
	}
	
	return &weaponPreviews[currentWeaponIndex];
}

int32 AOutGameWeaponPreviewManager::GetCurrentWeaponIndex() const{
	return currentWeaponIndex;
}

const FOutGameWeaponPreviewData* AOutGameWeaponPreviewManager::GetWeaponDataByIndex(int32 weaponIndex) const{
	if (weaponPreviews.IsValidIndex(weaponIndex) == false)
	{
		return nullptr;
	}

	return &weaponPreviews[weaponIndex];
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

	if (IsValid(currentWeaponActor) == true)
	{
		currentWeaponActor->SetActorScale3D(FVector(previewWeaponScale));
	}
	
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

void AOutGameWeaponPreviewManager::RotateCurrentWeapon(float deltaYaw, float deltaPitch){
	if (IsValid(currentWeaponActor) == false) return;
	
	FRotator currentRotation = currentWeaponActor->GetActorRotation();
	
	currentRotation.Yaw += deltaYaw * 0.2f;
	currentRotation.Pitch = FMath::Clamp(
		currentRotation.Pitch + deltaPitch * 0.1f,
		-45.0f,
		45.0f
	);
	
	currentWeaponActor->SetActorRotation(currentRotation);
}

