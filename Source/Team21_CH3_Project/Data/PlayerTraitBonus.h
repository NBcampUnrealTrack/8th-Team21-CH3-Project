#pragma once

#include "CoreMinimal.h"
#include "PlayerTraitBonus.generated.h"

USTRUCT(BlueprintType)
struct TEAM21_CH3_PROJECT_API FPlayerTraitBonus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trait|Effect")
	float reloadSpeedBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trait|Effect")
	float maxHPBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trait|Effect")
	float weaponDamageBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trait|Effect")
	float goldGainBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trait|Effect")
	float moveSpeedBonus = 0.f;
};
