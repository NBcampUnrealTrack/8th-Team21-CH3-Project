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
	int32 RequiredTotalKillCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> weaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> previewSpawnPoint;
	UPROPERTY(EditInstanceOnly)
	float weaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ammoCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float firePerMinute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float maxAttackRange;
};
