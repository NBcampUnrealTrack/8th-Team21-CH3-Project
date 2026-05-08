// OutGameWeaponPreviewData.h
#pragma once
#include "Game/TeamGameInstance.h"
#include "CoreMinimal.h"
#include "OutGameWeaponPreviewData.generated.h"

USTRUCT(BlueprintType)
struct TEAM21_CH3_PROJECT_API FOutGameWeaponPreviewData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType weaponType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText displayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> weaponClass;
	UPROPERTY(EditInstanceOnly)
	float weaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ammoCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float shootingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float shootingRange;
};
