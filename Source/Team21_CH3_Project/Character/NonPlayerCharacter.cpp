// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NonPlayerCharacter.h"
#include "Controller/AI_Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/StatusComponent.h"
#include "Animation/CharacterAnimInstance.h"
#include "Item/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Component/PickupComponent.h"
#include "Team21_CH3_Project.h"
#include "InGameUI/ShooterInGameMode.h"

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
		GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
		if (bAttackRange == true)
		{
			GetWeapon(RifleClass);
		}
	}
}

void ANonPlayerCharacter::BeginAttack()
{

	UAnimInstance* AnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (IsValid(AnimInstance) == true && IsValid(AttackMeleeMontage) == true && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false && bAttackRange == false)
	{

		AnimInstance->Montage_Play(AttackMeleeMontage);


		bIsNowAttacking = true;

		if (OnAttackMontageEndedDelegate.IsBound() == false)
		{
			OnAttackMontageEndedDelegate.BindUObject(this, &ANonPlayerCharacter::EndAttack);
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEndedDelegate, AttackMeleeMontage);

		}
	}

	if (bAttackRange == true)
	{
		TryFire();
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
			bool bNPCWin = false;
			GameMode->OnCharacterDied(bNPCWin);
			SetLifeSpan(0.1f);
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

void ANonPlayerCharacter::TryFire()
{
	AAI_Controller* AIController = GetController<AAI_Controller>();
	if (bAttackRange == false)
	{
		return;
	}
	if (IsValid(AIController) == true)
	{
		float FocalDistance = 400.f;
		FVector FocalLocation;
		FVector CameraLocation;
		FRotator CameraRotation;

		AIController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector AimDirectionFromCamera = CameraRotation.Vector().GetSafeNormal();
		FocalLocation = CameraLocation + (AimDirectionFromCamera * FocalDistance);

		FVector WeaponMuzzleLocation = CurrentWeapon->GetPickupComponent()->GetSocketLocation(TEXT("MuzzleFlash"));
		FVector FinalFocalLocation = FocalLocation + (((WeaponMuzzleLocation - FocalLocation) | AimDirectionFromCamera) * AimDirectionFromCamera);

		FTransform TargetTransform = FTransform(CameraRotation, FinalFocalLocation);

		if (1 == ShowAttackRangedDebug)
		{
			DrawDebugSphere(GetWorld(), WeaponMuzzleLocation, 2.f, 16, FColor::Red, false, 60.f);

			DrawDebugSphere(GetWorld(), CameraLocation, 2.f, 16, FColor::Yellow, false, 60.f);

			DrawDebugSphere(GetWorld(), FinalFocalLocation, 2.f, 16, FColor::Magenta, false, 60.f);

			DrawDebugLine(GetWorld(), FinalFocalLocation, WeaponMuzzleLocation, FColor::Red, false, 60.f, 0, 2.f);

			DrawDebugLine(GetWorld(), CameraLocation, FinalFocalLocation, FColor::Red, false, 60.f, 0, 2.f);

			DrawDebugLine(GetWorld(), WeaponMuzzleLocation,FinalFocalLocation, FColor::Red, false, 60.f, 0, 2.f);
		}
		FVector BulletDirection = TargetTransform.GetUnitAxis(EAxis::X);
		FVector StartLocation = WeaponMuzzleLocation;
		FVector EndLocation = TargetTransform.GetLocation() + BulletDirection * CurrentWeapon->GetMaxAttackRange();

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(NAME_None, false, this);
		TraceParams.AddIgnoredActor(CurrentWeapon);

		bool IsCollided = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_ATTACK, TraceParams);
		if (IsCollided == false)
		{
			HitResult.TraceStart = StartLocation;
			HitResult.TraceEnd = EndLocation;
		}
		if (2 == ShowAttackRangedDebug)
		{
			if (IsCollided == true)
			{
				DrawDebugSphere(GetWorld(), StartLocation, 2.f, 16, FColor::Red, false, 60.f);

				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 2.f, 16, FColor::Green, false, 60.f);

				DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Blue, false, 60.f, 0, 16);

			}
			else
			{

				DrawDebugSphere(GetWorld(), StartLocation, 2.f, 16, FColor::Red, false, 60.f);

				DrawDebugSphere(GetWorld(), EndLocation, 2.f, 16, FColor::Green, false, 60.f);

				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 60.f, 0, 16);
			}
		}

		if (IsCollided == true)
		{
			ACharacterBase* HittedCharacter = Cast<ACharacterBase>(HitResult.GetActor());
			if (IsValid(HittedCharacter) == true)
			{
				FDamageEvent DamageEvent;
				HittedCharacter->TakeDamage(Damage, DamageEvent, GetController(), this);
			}
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (IsValid(AnimInstance) == true)
		{
			if (AnimInstance->Montage_IsPlaying(GetCurrentWeaponAttackAnimMontage()) == false)
			{
				AnimInstance->Montage_Play(GetCurrentWeaponAttackAnimMontage());
			}
		}

	}
}