// OutGameWeaponPreviewManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OutGameUI/Data/OutGameWeaponPreviewData.h"
#include "OutGameWeaponPreviewManager.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API AOutGameWeaponPreviewManager : public AActor{
	GENERATED_BODY()

public:
	AOutGameWeaponPreviewManager();
	
	const FOutGameWeaponPreviewData* GetCurrentWeaponData();
	
	UFUNCTION()
	bool ShowWeaponByIndex(int32 weaponIndex);
	UFUNCTION()
	bool ShowNextWeapon();
	UFUNCTION()
	bool ShowPreviousWeapon();
	UFUNCTION()
	void ClearPreviewWeapon();
	UFUNCTION()
	void SetWeaponIndex(int32 index);
	UFUNCTION()
	void RotateCurrentWeapon(float deltaYaw, float deltaPitch);
	
private:
	UPROPERTY(EditAnywhere, Category = "Preveiw|Weapon")
	TArray<FOutGameWeaponPreviewData> weaponPreviews;
	UPROPERTY(EditAnywhere, Category = "Preview|Weapon")
	float previewWeaponScale;
	
	int32 currentWeaponIndex;
	AActor* currentWeaponActor;
	AActor* previewSpawnPoint;
};
