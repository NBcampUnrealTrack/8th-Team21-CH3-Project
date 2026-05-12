

#include "Character/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/InputConfig.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Component/PickupComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Team21_CH3_Project.h"
#include "Animation/CharacterAnimInstance.h"
#include "Interfaces/Interaction.h"
#include "Game/TeamGameInstance.h"
#include "Component/StatusComponent.h"
#include "ShooterInGameMode.h"
#include "InGameUI/InGameHUD.h"
#include "Kismet/GameplayStatics.h"



APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 550.f;
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bDoCollisionTest = true;
		//SetRelativeLocation은 BP에서 조정

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	//TimeBetweenFire = 60.f / FirePerMinute;  
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	 
	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem))
		{
			Subsystem->AddMappingContext(CharacterIMC, 0);
			//UE_LOG(LogTemp, Warning, TEXT("AddMappingContext Suceess"));
		}
	}

	if (IsValid(StatusComponent))
	{
		// 최대체력이 변하면 MaxHP에 관련된 함수들에게 알림
		StatusComponent->OnMaxHPChanged.AddUObject(this, &APlayerCharacter::HandleMaxHPChanged);

		// 현재체력의 변화를 알림
		StatusComponent->OnCurrentHPChanged.AddUObject(this, &APlayerCharacter::HandleCurrentHPChanged);

		// 체력이 0이 되었음을 알림
		StatusComponent->OnOutOfCurrentHP.AddUObject(this, &APlayerCharacter::HandleOutOfCurrentHP);

		// 게임 시작 시 현재 HP 값을 HUD에 즉시 반영한다.
		RefreshPlayerHealthUI();
	}
	//CurrentWeapon = nullptr;
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(StatusComponent))
	{
		StatusComponent->OnMaxHPChanged.RemoveAll(this);
		StatusComponent->OnCurrentHPChanged.RemoveAll(this);
		StatusComponent->OnOutOfCurrentHP.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 25.f);
	CameraComp->SetFieldOfView(CurrentFOV);
	 
	//CurrentSpeed = FMath::FInterpTo(CurrentSpeed, TargetSpeed, DeltaSeconds, 20.f);
	//GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Super::TakeDamage 내부에서 StatusComponent 체력이 변경되면
	// OnCurrentHPChanged 델리게이트를 통해 HUD가 자동 갱신된다.
	RefreshPlayerHealthUI();

	return FinalDamageAmount;
}

void APlayerCharacter::HandleMaxHPChanged(float InMaxHP)
{
	RefreshPlayerHealthUI();
}

void APlayerCharacter::HandleCurrentHPChanged(float InCurrentHP)
{
	RefreshPlayerHealthUI();
}

void APlayerCharacter::HandleOutOfCurrentHP()
{
	RefreshPlayerHealthUI();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
		if (IsValid(InGameHUD))
		{
			// 플레이어가 사망하면 위험 피드백을 제거한다.
			InGameHUD->HideHPDangerFeedback();
		}
	}

	AShooterInGameMode* InGameMode = Cast<AShooterInGameMode>(UGameplayStatics::GetGameMode(this));
	if (IsValid(InGameMode))
	{
		// 플레이어가 사망했으므로 true 전달
		InGameMode->OnCharacterDied(true);
	}
}

void APlayerCharacter::RefreshPlayerHealthUI()
{
	if (!IsValid(StatusComponent))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
	if (!IsValid(InGameHUD))
	{
		return;
	}

	InGameHUD->RefreshHealthUI(
		StatusComponent->GetCurrentHP(),
		StatusComponent->GetMaxHP()
	);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* CharacterInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(CharacterInputComponent))
	{
		CharacterInputComponent->BindAction(CharacterInputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		CharacterInputComponent->BindAction(CharacterInputConfig->Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
		CharacterInputComponent->BindAction(CharacterInputConfig->Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		CharacterInputComponent->BindAction(CharacterInputConfig->Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		CharacterInputComponent->BindAction(CharacterInputConfig->AttackRanged, ETriggerEvent::Started, this, &ThisClass::InputAttackRanged);
		CharacterInputComponent->BindAction(CharacterInputConfig->AttackMelee, ETriggerEvent::Started, this, &ThisClass::InputAttackMelee);
		CharacterInputComponent->BindAction(CharacterInputConfig->Zoom, ETriggerEvent::Started, this, &ThisClass::InputStartZoom);
		CharacterInputComponent->BindAction(CharacterInputConfig->Zoom, ETriggerEvent::Completed, this, &ThisClass::InputEndZoom);
		CharacterInputComponent->BindAction(CharacterInputConfig->Dash, ETriggerEvent::Started, this, &ThisClass::InputStartDash);
		CharacterInputComponent->BindAction(CharacterInputConfig->Dash, ETriggerEvent::Completed, this, &ThisClass::InputEndDash);
		CharacterInputComponent->BindAction(CharacterInputConfig->ToggleSelector, ETriggerEvent::Started, this, &ThisClass::InputToggleSelector);
		CharacterInputComponent->BindAction(CharacterInputConfig->AttackRanged, ETriggerEvent::Started, this, &ThisClass::InputStartFullAutoFire);
		CharacterInputComponent->BindAction(CharacterInputConfig->AttackRanged, ETriggerEvent::Completed, this, &ThisClass::InputStopFullAutoFire);
		CharacterInputComponent->BindAction(CharacterInputConfig->Interaction, ETriggerEvent::Started, this, &ThisClass::InputInteraction);
		//UE_LOG(LogTemp, Warning, TEXT("InputComponent Bind Suceess"));
	}
}

void APlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	FVector2D MoveVector = InValue.Get<FVector2D>();

	const FRotator CharacterRotation = GetController()->GetControlRotation();
	const FRotator CharacterRotationYaw(0.f, CharacterRotation.Yaw, 0.f);

	const FVector FowardVector = FRotationMatrix(CharacterRotationYaw).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(CharacterRotationYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(FowardVector,MoveVector.X);
	AddMovementInput(RightVector, MoveVector.Y);
}

void APlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	FVector2D LookVector = InValue.Get<FVector2D>();

	float Sensitivity = 1.f;
	UTeamGameInstance* GameInstance = Cast<UTeamGameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		Sensitivity = GameInstance->GetMouseSensitivity();
	}

	AddControllerYawInput(LookVector.X * Sensitivity);
	AddControllerPitchInput(LookVector.Y * Sensitivity);
}

void APlayerCharacter::InputAttackRanged(const FInputActionValue& InValue)
{
	if (0.f < GetCharacterMovement()->Velocity.Size())
		//캐릭터의 속도(벡터의 크기)가 0이상이면 -> 움직이고 있다면
	{
		return; //코드 실행 X
	}
	
	if (IsValid(CurrentWeapon) == false) // 무기를 줍지 않았다면
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon이 nullptr"));
		return; //코드 실행 X
	}
	
	if (IsValid(GetCurrentWeaponAttackAnimMontage()) == false) //애님몽타주가 연결X라면
	{
		//UE_LOG(LogTemp, Warning, TEXT("AttackMontage가 nullptr"));
		return; //코드 실행 X
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("사격 조건 통과"));
	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); //현재 클래스의 메시(캐릭터)의 애님인스턴스
	//if (IsValid(AnimInstance) == true) //가 있다면
	//{
	//	if (AnimInstance->Montage_IsPlaying(GetCurrentWeaponAttackAnimMontage()) == false) //애님몽타주가 실행중이 아니라면
	//	{
	//		AnimInstance->Montage_Play(GetCurrentWeaponAttackAnimMontage()); //해당 애님 몽타주 실행
	//	}
	//}

	//APlayerController* OwnerPlayerController = Cast<APlayerController>(GetController());
	//if (IsValid(AttackRangedCameraShake) && IsValid(OwnerPlayerController))
	//{
	//	OwnerPlayerController->ClientStartCameraShake(AttackRangedCameraShake);
	//}

	if (false == bIsFullAutoFire)
	{
		TryFire();
	}

}

void APlayerCharacter::InputAttackMelee(const FInputActionValue& InValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("=== InputAttackMelee 호출됨 ==="));

	if (GetCharacterMovement()->IsFalling() == true)
	{
		//UE_LOG(LogTemp, Warning, TEXT("실패: 공중 상태"));
		return;
	}
	
	UCharacterAnimInstance* AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	
	//UE_LOG(LogTemp, Warning, TEXT("AnimInstance 유효: %s"), IsValid(AnimInstance) ? TEXT("O") : TEXT("X"));
	//UE_LOG(LogTemp, Warning, TEXT("AttackMeleeMontage 유효: %s"), IsValid(AttackMeleeMontage) ? TEXT("O") : TEXT("X"));

	if (IsValid(AnimInstance) == true && IsValid(AttackMeleeMontage) == true && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false)
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);
		//UE_LOG(LogTemp, Warning, TEXT("몽타주 재생 시작"));
	}
}

void APlayerCharacter::TryFire()
{

	if (CurrentWeapon->UseBullets() == false)
	{
		GetWorldTimerManager().ClearTimer(FullAutoTimerHandle);
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("TryFire at: %f"), GetWorld()->GetTimeSeconds());
	APlayerController* PlayerController = GetController<APlayerController>();
	if (IsValid(PlayerController) == true)
	{
#pragma region CaculateTargetTransform
		float FocalDistance = 400.f;
		FVector FocalLocation;
		FVector CameraLocation;
		FRotator CameraRotation;

		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

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

			// (WeaponLoc - FocalLoc)
			DrawDebugLine(GetWorld(), FocalLocation, WeaponMuzzleLocation, FColor::Yellow, false, 60.f, 0, 2.f);

			// AimDir
			DrawDebugLine(GetWorld(), CameraLocation, FinalFocalLocation, FColor::Blue, false, 60.f, 0, 2.f);

			// Project Direction Line
			DrawDebugLine(GetWorld(), WeaponMuzzleLocation, FinalFocalLocation, FColor::Red, false, 60.f, 0, 2.f);
		}

#pragma endregion

#pragma region PerformLineTracing

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

				DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Blue, false, 60.f, 0, 2.f);
			}
			else
			{
				DrawDebugSphere(GetWorld(), StartLocation, 2.f, 16, FColor::Red, false, 60.f);

				DrawDebugSphere(GetWorld(), EndLocation, 2.f, 16, FColor::Green, false, 60.f);

				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, 60.f, 0, 2.f);
			}
		}

#pragma endregion

		if (IsCollided == true)
		{
			ACharacterBase* HittedCharacter = Cast<ACharacterBase>(HitResult.GetActor());
			if (IsValid(HittedCharacter) == true)
			{
				FDamageEvent DamageEvent;
				HittedCharacter->TakeDamage(10.f, DamageEvent, GetController(), this);
			}
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (IsValid(AnimInstance) == true)
		{
			if (AnimInstance->Montage_IsPlaying(GetCurrentWeaponAttackAnimMontage()) == false)
			{
				float MontageLength = GetCurrentWeaponAttackAnimMontage()->GetPlayLength();
				float PlayRate = MontageLength / (60.f / FirePerMinute);
				AnimInstance->Montage_Play(GetCurrentWeaponAttackAnimMontage(), PlayRate);
			}
		}

		if (IsValid(AttackRangedCameraShake) == true)
		{
			PlayerController->ClientStartCameraShake(AttackRangedCameraShake);
		}
	}
}


void APlayerCharacter::InputStartZoom(const FInputActionValue& InValue)
{
	TargetFOV = 45.f;
}

void APlayerCharacter::InputEndZoom(const FInputActionValue& InValue)
{
	TargetFOV = 70.f;
}

void APlayerCharacter::InputStartDash(const FInputActionValue& InValue)
{
	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
	GetCharacterMovement()->MaxAcceleration = 5000.f;
}

void APlayerCharacter::InputEndDash(const FInputActionValue& InValue)
{
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	GetCharacterMovement()->MaxAcceleration = CurrentAcceleration;
}

void APlayerCharacter::InputToggleSelector(const FInputActionValue& InValue)
{
	bIsFullAutoFire = !bIsFullAutoFire;
}

void APlayerCharacter::InputStartFullAutoFire(const FInputActionValue& InValue)
{
	if (true == bIsFullAutoFire)
	{
		if (GetWorldTimerManager().IsTimerActive(FullAutoTimerHandle) == false)
		{
			float TimeBetweenFire = 60.f / FirePerMinute;
			//UE_LOG(LogTemp, Warning, TEXT("TimeBetweenFire: %f, FirePerMinute: %f"), TimeBetweenFire, FirePerMinute);
			GetWorldTimerManager().SetTimer(FullAutoTimerHandle, this, &ThisClass::TryFire, TimeBetweenFire, true);
		}
	}
}

void APlayerCharacter::InputStopFullAutoFire(const FInputActionValue& InValue)
{
	if (true == bIsFullAutoFire)
	{
		GetWorldTimerManager().ClearTimer(FullAutoTimerHandle);
	}
}

void APlayerCharacter::InputInteraction(const FInputActionValue& InValue)
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), 50.f, 16, FColor::Yellow, false, 3.f);

	APlayerController* PlayerController = GetController<APlayerController>();
	if (IsValid(PlayerController) == false)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceViewPoint = CameraLocation + CameraRotation.Vector() * InteractionRange;

	FHitResult HitResult;
	FCollisionQueryParams TraceCollision(NAME_None, false, this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceViewPoint, ECC_Visibility, TraceCollision);

	if (bHit)
	{
		DrawDebugLine(GetWorld(), CameraLocation, HitResult.ImpactPoint, FColor::Green, false, 3.f, 0, 2.f);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 5.f, 16, FColor::Green, false, 3.f);

		IInteraction* Interactable = Cast<IInteraction>(HitResult.GetActor());
		if (Interactable != nullptr)
		{
			Interactable->Interact(this);
			//UE_LOG(LogTemp, Warning, TEXT("Interaction Activated"))
		}
		
	}

	else
	{
		// 아무것도 안 맞았을 때: 시작점 → 끝점까지 빨간선
		DrawDebugLine(GetWorld(), CameraLocation, TraceViewPoint, FColor::Red, false, 3.f, 0, 2.f);
	}
}
