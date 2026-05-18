#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/TeamGameInstance.h"
#include "Weapon.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32 /*CurrentBullets*/, int32 /*MaxBullets*/);

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

	UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

	float GetMaxAttackRange() const { return MaxAttackRange; }

	int32 GetCurrentBullets() const { return CurrentBullets; }
	void SetCurrentBullsets(int32 InCurrentBullsets);

	int32 GetMaxBullets() const { return MaxBullets; }

	bool GetCanFullAuto() const { return bCanFullAuto; }

	int32 GetBulletsCount() const { return BulletsCount; }

	float GetSpreadAngle() const { return SpreadAngle; }

	float GetAttackDamage() const { return AttackDamage; }

	void EquipToCharacter(ACharacterBase* InCharacter);

	void Reload();

	bool UseBullets();

public:
	FOnAmmoChanged OnAmmoChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnPickUp(ACharacterBase* InPickUpCharacter);



protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPickupComponent> PickupComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage; //리로드 애님몽타주

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Units = cm))
	float MaxAttackRange = 25000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::None; //무기 타입 설정

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets")
	int32 MaxBullets = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets")
	int32 CurrentBullets = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	uint8 bCanFullAuto : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 BulletsCount = 1; //한번에 발사되는 탄환 수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float SpreadAngle = 0.f; // 탄환 퍼지는 각도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float FirePerMinute = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackDamage = 50.f;
};