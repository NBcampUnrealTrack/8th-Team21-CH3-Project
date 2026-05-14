// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "AI/Monster/MonsterStat.h"
#include "NonPlayerCharacter.generated.h"


DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded, UAnimMontage*, bool /*bInterrupted*/)

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

	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

	void SpawnMonster(TSubclassOf<ACharacterBase> InSpawnMonster);

public:
	bool bIsNowAttacking;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TSubclassOf<ACharacterBase> Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TSubclassOf<ACharacterBase> Rusher;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster")
	TSubclassOf<ACharacterBase> Shooter;

	UPROPERTY(BlueprintReadOnly, Category = "Monster")
	FMonsterStat SetMonster;

	FOnAttackMontageEnded OnAttackMontageEndedDelegate;
	float LastUpdatedMaxHP = SetMonster.MaxHP;
	float LastUpdatedCurrentHP = 0.f;
};
