

#include "Animation/CharacterAnimInstance.h"
#include "Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h" //여러 수학적 로직을 가지고있는 Static 클래스
#include "Character/PlayerCharacter.h"
#include "Character/NonPlayerCharacter.h"
#include "Component/StatusComponent.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	APawn* OwnerPawn = TryGetPawnOwner();

	if (IsValid(OwnerPawn))
	{
		OwnerCharacter = Cast<ACharacterBase>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
	bIsUnarmed = true;
	bIsMovingBackward = false;
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(OwnerCharacter) && IsValid(OwnerCharacterMovement)) //캐릭터가 있고, movementcomponent도 있다면
	{
		Velocity = OwnerCharacterMovement->Velocity;

		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		// Z는 무시하고XY값만 사용
		// 점프 중에도 XY값만 적용

		Direction = CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorRotation());

		bIsMove = (KINDA_SMALL_NUMBER < GroundSpeed);
		bIsFalling = OwnerCharacterMovement->IsFalling();
		bIsUnarmed = OwnerCharacter->GetCurrentWeaponAttackAnimMontage() == nullptr ? true : false;
		float GroundAcceleration = UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());
		bool bIsAccelerated = FMath::IsNearlyZero(GroundAcceleration) == false;
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && (bIsAccelerated == true);
		bIsMovingBackward = Direction > 135.f || Direction < -135.f;
		if (ANonPlayerCharacter* OwnerNPC = Cast<ANonPlayerCharacter>(OwnerCharacter))
		{
			bShouldMove = KINDA_SMALL_NUMBER < GroundSpeed;
		}

		
		if (APlayerController* OwnerPlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			NormalizedCurrentPitch = UKismetMathLibrary::NormalizeAxis(OwnerPlayerController->GetControlRotation().Pitch);
			NormalizedCurrentYaw = UKismetMathLibrary::NormalizeAxis(OwnerPlayerController->GetControlRotation().Yaw);
		}
		//bIsDead = OwnerCharacter->IsDead();
		if (IsValid(OwnerCharacter->GetStatusComponent()) == true)
		{
			bIsDead = OwnerCharacter->GetStatusComponent()->IsDead();
		}

		if (bIsUnarmed)
		{
			WeaponTypeIndex = 0;
		}
		else
		{
			switch (OwnerCharacter->CurrentWeaponType)
			{
				case EWeaponType::Rifle:
					WeaponTypeIndex = 1;
					break;
				case EWeaponType::Shotgun:
					WeaponTypeIndex = 2;
					break;
				case EWeaponType::Pistol:
					WeaponTypeIndex = 3;
					break;
				default:
					WeaponTypeIndex = 0;
					break;
			}
		}
	}
}

void UCharacterAnimInstance::AnimNotify_PostDead()
{
	if (OnPostDead.IsBound() == true)
	{
		OnPostDead.Broadcast();
	}
}
