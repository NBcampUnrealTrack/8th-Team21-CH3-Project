#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TraitData.generated.h"

class UTexture2D;

UENUM(Blueprintable)
enum class ETraitCardState : uint8{
	Invalid,
	Locked,
	Upgradeable,
	NotEnoughGold,
	MaxLevel
};

UENUM(BlueprintType)
enum class ETraitCategory : uint8
{
	None,
	Attack,
	Survival,
	Mobility
};

USTRUCT(BlueprintType)
struct TEAM21_CH3_PROJECT_API FTraitData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETraitCategory TraitCategory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Tier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RequiredTraitId;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait")
	FText traitName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait")
	FText description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Effect")
	float reloadSpeedBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Effect")
	float maxHPBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Effect")
	float weaponDamageBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Effect")
	float goldGainBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Effect")
	float moveSpeedBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Unlock")
	int32 unlockGoldCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait|Visual")
	TObjectPtr<UTexture2D> icon = nullptr;
};
