
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class AWeapon;
class UAnimMontage;
class UStatusComponent;
class AShooterInGameMode

UCLASS()
class TEAM21_CH3_PROJECT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void BeginPlay() override;

	virtual float TakeDamage(
		float DamageAmount, //데미지 세기
		struct FDamageEvent const& DamageEvent, //데미지 종류 
		class AController* EventInstigator, //공격 컨트롤러
		AActor* DamageCauser //공격 액터(투사체,무기...)
	) override;

	UAnimMontage* GetCurrentWeaponAttackAnimMontage() const;

	UFUNCTION()
	void HandleOnCheckHit();


public:
	UPROPERTY()
	TObjectPtr<AWeapon> CurrentWeapon;

	static int32 ShowAttackMeleeDebug;

	static int32 ShowAttackRangedDebug;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMeleeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackMeleeRange = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackMeleeRadius = 20.f;

#pragma region HP

public:
	//float GetMaxHP() const { return MaxHP; }
	//	//Getter MaxHP
	//float GetCurrentHP() const { return CurrentHP; }
	//	//Getter CurrentHP
	//void SetMaxHP(float InMaxHP) { MaxHP = InMaxHP; }
	//	//Setter MaxHP
	//void SetCurrentHP(float InCurrentHP) { CurrentHP = InCurrentHP; }
	//	//Setter CurrenHP
	//bool IsDead() const { return bIsDead; }
	//	//죽었는가?(CurrentHP가 0이 되었는가?)

	UStatusComponent* GetStatusComponent() const { return StatusComponent; }

protected:
	UFUNCTION()
	virtual void HandleOnPostCharacterDead(); //개릭터 사망 후 로직
protected:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//float MaxHP = 100.f;
	//
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//float CurrentHP = 100.f;
	//
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//uint8 bIsDead : 1;
		//사용메모리 1바이트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UStatusComponent> StatusComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<AShooterInGameMode> GameMode;

#pragma endregion

};
