// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Character/PlayerCharacter.h"
#include "NonPlayerCharacter.generated.h"


DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded, UAnimMontage*, bool /*bInterrupted*/)
DECLARE_DELEGATE(FChargeCoolTime)

class UStatusComponent;
/**
 * 
 */
UCLASS()
class TEAM21_CH3_PROJECT_API ANonPlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

	friend class UBTTask_Attack;
	
public:
	ANonPlayerCharacter();

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	void InitializeHP(UStatusComponent* InStatusComponent);

	UFUNCTION()
	void OnMaxHPChange(float InMaxHP);
	UFUNCTION()
	void OnCurrentHPChange(float InCurrentHP);

protected:
	virtual void BeginAttack();

	void TryFire();
	//АјАн
	void EndAttack();

	void POW(bool bVulnerable);
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);
	void ChangebIsCharge();

	UFUNCTION()
	void OnBossCapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	bool bIsNowAttacking;
	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	bool bAttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MoveSpeed = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MaxHP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float Damage =5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	bool bInvulnerable;
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsCharging = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ChargeDamage = 80.f;
	FChargeCoolTime CoolTime;
	UPROPERTY()
	TArray<AActor*> HitTargets;
protected:
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;
	float LastUpdatedMaxHP = 0.f;
	float LastUpdatedCurrentHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AWeapon>WeaponType;
	UPROPERTY(VisibleAnywhere)
	UStatusComponent* Status;

};
