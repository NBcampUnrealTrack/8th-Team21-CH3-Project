
#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputConfig;
class UInputMappingContext;

UCLASS()
class TEAM21_CH3_PROJECT_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:

	APlayerCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta =(AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComp;

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


protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta =(AllowPrivateAccess))
	TObjectPtr<UInputConfig> CharacterInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta =(AllowPrivateAccess))
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetSpeed = 1200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentSpeed = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
};
