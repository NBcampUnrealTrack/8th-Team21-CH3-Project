#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputConfig;
class UInputMappingContext;
class UAugmentComponent;
class UDataTable;
struct FPlayerTraitBonus;

UCLASS()
class TEAM21_CH3_PROJECT_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:

	APlayerCharacter();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComp;

#pragma endregion

#pragma region Player HP UI

protected:
	// 최대체력이 변경되었을 때 HUD 체력 UI를 갱신한다.
	void HandleMaxHPChanged(float InMaxHP);

	// 현재체력이 변경되었을 때 HUD 체력 UI를 갱신한다.
	void HandleCurrentHPChanged(float InCurrentHP);

	// 체력이 0이 되었을 때 라운드 사망 처리를 호출한다.
	void HandleOutOfCurrentHP();

	// 현재 StatusComponent 값을 InGameHUD에 반영한다.
	void RefreshPlayerHealthUI();

#pragma endregion

#pragma region Input

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);

	void InputAttackRanged(const FInputActionValue& InValue);
	void InputAttackMelee(const FInputActionValue& InValue);
	void TryFire();
	void InputStartZoom(const FInputActionValue& InValue);
	void InputEndZoom(const FInputActionValue& InValue);
	void InputStartDash(const FInputActionValue& InValue);
	void InputEndDash(const FInputActionValue& InValue);
	void InputToggleSelector(const FInputActionValue& InValue);
	void InputStartFullAutoFire(const FInputActionValue& InValue);
	void InputStopFullAutoFire(const FInputActionValue& InValue);
	void InputInteraction(const FInputActionValue& InValue);
	void InputReLoad(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputConfig> CharacterInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> CharacterIMC;

#pragma endregion

#pragma region Effect

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> AttackRangedCameraShake;

#pragma endregion 

#pragma region Zoom

protected:
	float TargetFOV = 70.f;
	//Field Of View 시야 각

	float CurrentFOV = 70.f;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float TargetSpeed = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentAcceleration = 2048.f;

#pragma endregion

#pragma region Selector

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FirePerMinute = 600;
	//분당 발사

	bool bIsFullAutoFire = false;
	//연발 상태인가?

	FTimerHandle FullAutoTimerHandle;
	// 타이머 핸들 변수 선언

	//float TimeBetweenFire;
	// 발사 간 타이밍

#pragma endregion

#pragma region Interaction

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	float InteractionRange = 1000.f;

#pragma endregion

#pragma region Augment

public:
	void ApplyAugment_AttackDamage(float InAdd);
	void ApplyAugment_MoveSpeed(float InAdd);
	void ApplyAugment_MaxHP(float InAdd);
	void ApplyAugment_ItemCapacity(int32 InAdd);

public:
	// 공격력 배율 (기본 1.0 = 100%)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Augment")
	float AttackDamageMul = 1.0f;

	// 아이템 최대 보유 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Augment")
	int32 MaxItemCapacity = 3;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
	TObjectPtr<UAugmentComponent> AugmentComponent;

#pragma endregion

#pragma region Reload

private:
	// 몽타주 종료 델리게이트에 바인딩할 함수
	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reload")
	bool bIsReloading = false;

	UFUNCTION()
	void OnAmmoChanged(int32 CurrentBullets, int32 MaxBullets);

#pragma endregion

#pragma region Attribute

	float baseSpeed = 500.f;

	float baseAttackDamage;

	float baseTargetSpeed = 1000.f;

#pragma endregion

#pragma region Trait

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trait")
	TObjectPtr<UDataTable> TraitDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trait")
	float ReloadSpeedMul = 1.0f;

public:
	void ApplyTraitBonus(const FPlayerTraitBonus& Bonus);
#pragma endregion
};