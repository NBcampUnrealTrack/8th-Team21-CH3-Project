
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class ACharacterBase;
class UPickupComponent;
class UAnimMontage;

UCLASS()
class TEAM21_CH3_PROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	UPickupComponent* GetPickupComponent() const { return PickupComponent; }

	UAnimMontage* GetAttackMontage() const { return AttackMontage; }

	float GetMaxAttackRange() const { return MaxAttackRange; }
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnPickUp(ACharacterBase* InPickUpCharacter);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPickupComponent> PickupComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxAttackRange = 25000.f;

};
