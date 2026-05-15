// TraitSubSystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Trait/Data/TraitData.h"
#include "Data/PlayerTraitBonus.h"
#include "TraitSubsystem.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API UTraitSubsystem : public UGameInstanceSubsystem{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool CanUpgradeTrait(FName traitId, const FTraitData& traitData) const;
	UFUNCTION(BlueprintCallable)
	bool TryUpgradeTrait(FName traitId, const FTraitData& traitData);
	
private:
	UFUNCTION(BlueprintCallable)
	FPlayerTraitBonus CalculateTotalTraitBonus(FName traitId, const FTraitData& traitData);
};
