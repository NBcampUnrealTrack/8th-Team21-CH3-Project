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
	
	UFUNCTION()
	void ShowWeaponByIndex(int32 index);
	UFUNCTION()
	void ShowNextWeapon();
	UFUNCTION()
	void ShowPreviousWeapon();
	UFUNCTION()
	void ClearPreviewWeapon();
private:
	UPROPERTY(EditAnywhere, Category = "Preveiw|Weapon")
	TArray<FOutGameWeaponPreviewData> weaponPreviews;
	
	int32 currentWeaponIndex;
	AActor* currentWeaponActor;
	AActor* previewSpawnPoint;
};
