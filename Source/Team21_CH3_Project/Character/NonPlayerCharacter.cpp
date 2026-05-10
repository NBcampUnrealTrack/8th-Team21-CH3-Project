// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerCharacter.h"

#include "Controller/AI_Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/StatusComponent.h"
#include "Animation/CharacterAnimInstance.h"

ANonPlayerCharacter::ANonPlayerCharacter() : bIsNowAttacking(false)
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAI_Controller::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

	}
}

void ANonPlayerCharacter::BeginAttack()
{
	UAnimInstance* AnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (IsValid(AnimInstance) == true && IsValid(AttackMeleeMontage) == true && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false)
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);

		bIsNowAttacking = true;

		if (OnAttackMontageEndedDelegate.IsBound() == false)
		{
			OnAttackMontageEndedDelegate.BindUObject(this, &ANonPlayerCharacter::EndAttack);
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEndedDelegate, AttackMeleeMontage);
		}
	}

	
}

float ANonPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//if (CurrentHP < KINDA_SMALL_NUMBER)
	if (StatusComponent->IsDead() == true)
	{
		AAI_Controller* AIController = Cast<AAI_Controller>(GetController());
		if (IsValid(AIController) == true)
		{
			AIController->EndAI();
		}
	}
	return FinalDamageAmount;
}

void ANonPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	bIsNowAttacking = false;

	if (OnAttackMontageEndedDelegate.IsBound() == true)
	{
		OnAttackMontageEndedDelegate.Unbind();
	}
}

void ANonPlayerCharacter::InitializeHP(UStatusComponent* InStatusComponent)
{
	OnMaxHPChange(InStatusComponent->GetMaxHP());
	OnCurrentHPChange(InStatusComponent->GetCurrentHP());
}

void ANonPlayerCharacter::OnMaxHPChange(float InMaxHP)
{
	if (LastUpdatedMaxHP == InMaxHP)
	{
		return;
	}

	LastUpdatedMaxHP = InMaxHP;
}

void ANonPlayerCharacter::OnCurrentHPChange(float InCurrentHP)
{
	if (LastUpdatedCurrentHP == InCurrentHP)
	{
		return;
	}
	LastUpdatedCurrentHP = InCurrentHP;
}