

#include "Character/CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Weapon.h"
#include "Animation/CharacterAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Team21_CH3_Project.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Component/StatusComponent.h"
#include "ShooterInGameMode.h"

int32 ACharacterBase::ShowAttackMeleeDebug = 0;

FAutoConsoleVariableRef CVarShowAttackMeleeDebug(
	TEXT("SX.ShowAttackMeleeDebug"),
	ACharacterBase::ShowAttackMeleeDebug,
	TEXT(""),
	ECVF_Cheat
);

int32 ACharacterBase::ShowAttackRangedDebug = 0;

FAutoConsoleVariableRef CVarShowAttackRangedDebug(
	TEXT("SX.ShowAttackRangedDebug"),
	ACharacterBase::ShowAttackRangedDebug,
	TEXT(""),
	ECVF_Cheat
);

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	float CapsuleHeight = 90.f;
	float CapsuleRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CapsuleRadius, CapsuleHeight); //캡슐컴포넌트 Size

	FVector CharacterPosition(0.f, 0.f, -CapsuleHeight); //캐릭터 초기 위치
	FRotator CharacterRotation(0.f, -90.f, 0.f); //캐릭터 초기 회전방향
	GetMesh()->SetRelativeLocationAndRotation(CharacterPosition, CharacterRotation);//생성한 매시를 해당위치, 회전방향의 뷰포트에 위치	
	GetMesh()->SetCollisionProfileName(TEXT("MyCharacterMesh"));

	GetCharacterMovement()->MaxWalkSpeed = 500.f; //이동속도
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f; //최저속도(이동시작시 속도)
	GetCharacterMovement()->JumpZVelocity = 600.f; // 점프시 튕겨 올라가는 속도
	GetCharacterMovement()->AirControl = 0.5f; //공중에서 컨트롤 정도
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; // 이동키를 뗐을때 감속속도
	GetCharacterMovement()->MaxAcceleration = 2048.f;

	//bIsDead = false;
	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent")); //StatusComponent를 CHaracterrBase에 컴포넌트로 부착
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AShooterInGameMode>(GetWorld()->GetAuthGameMode());

	UTeamGameInstance* GameInstance = Cast<UTeamGameInstance>(GetGameInstance());
	if (IsValid(GameInstance) == false)
	{
		return;
	}
	TSubclassOf<AWeapon> SelectWeapon = nullptr;
	EWeaponType SelectType = GameInstance->GetSelectedWeaponType();

	if (SelectType == EWeaponType::Rifle)
	{
		SelectWeapon = RifleClass;
	}
	else if (SelectType == EWeaponType::Shotgun)
	{
		SelectWeapon = ShotgunClass;
	}
	else if (SelectType == EWeaponType::Pistol)
	{
		SelectWeapon = PistolClass;
	}
	else
		SelectWeapon = RifleClass;
	
	GetWeapon(SelectWeapon);
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//부모클래스의 TakDamage가져와서 사용
	
	// CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);
	StatusComponent->ApplyDamage(FinalDamageAmount);

	//if (CurrentHP < KINDA_SMALL_NUMBER)
	if (StatusComponent->IsDead() == true)
		// KINDA_SMALL_NUMBER == 0.0001, 0이 아니더라도 0.0001이되면 죽음으로 간주
		// 0.0f로 하면 컴퓨터가 미세하게 0.000001이렇게 값을 남길때도 있다.
		// 예기치 못할 오류 방지
	{
		//bIsDead = true; //GG
		//CurrentHP = 0.f; //현재 체력 0
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// 캡슐컴포넌트 NoCollision으로 설정
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		// 캐릭터 못움직이게 설정
	}

	if (1 == ShowAttackMeleeDebug)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f / %.1f]"), *GetName(), StatusComponent->GetCurrentHP(), StatusComponent->GetMaxHP()));
	}

	return FinalDamageAmount;
}

UAnimMontage* ACharacterBase::GetCurrentWeaponAttackAnimMontage() const
{
	if (IsValid(CurrentWeapon) == true)
	{
		return CurrentWeapon->GetAttackMontage();
	}
	return nullptr;
}

void ACharacterBase::HandleOnCheckHit()
{
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + AttackMeleeRange * GetActorForwardVector(),
		FQuat::Identity,
		ECC_ATTACK,
		FCollisionShape::MakeSphere(AttackMeleeRadius),
		Params
	);

	if (true == bResult)
	{
		if (HitResults.IsEmpty() == false)
		{
			for (const FHitResult& HitResult : HitResults)
			{
				if (IsValid(HitResult.GetActor()) == true)
				{
					if (1 == ShowAttackMeleeDebug)
					{
						UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
					}
					FDamageEvent DamageEvent;
					HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);
				}
			}
		}
	}
	
	//if (1 == ShowAttackMeleeDebug)
	//{
	//	FVector TraceVector = AttackMeleeRange * GetActorForwardVector(); //50.f * 벡터X , 정면으로 나가는 벡터
	//	FVector Center = GetActorLocation() + TraceVector + GetActorUpVector() * 40.f; //캐릭터 앞쪽이동/캡슐을 위로 40.f들어올림
	//	float HalfHeight = AttackMeleeRange * 0.5f + AttackMeleeRadius; //캡슐높이 절반
	//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVector).ToQuat(); //캡슐 회전 / ToQuit-> 가로로 눕힘
	//	FColor DrawColor = true == bResult ? FColor::Green : FColor::Red; //초록색 : hit, 빨간색 NonHit
	//	float DebugLifeTime = 5.f; //Duration
	//
	//	DrawDebugCapsule(
	//		GetWorld(), //디버깅할 장소
	//		Center, //캡슐 중앙위치
	//		HalfHeight, //캡슐 반높이
	//		AttackMeleeRadius,//반지름
	//		CapsuleRot,//가로
	//		DrawColor,
	//		false, //영구 지속
	//		DebugLifeTime
	//	);
	//}
}

void ACharacterBase::HandleOnPostCharacterDead()
{
	SetLifeSpan(0.1f);
	//0.1초뒤 메모리에서 삭제(Destroy())
}

void ACharacterBase::GetWeapon(TSubclassOf<AWeapon> InWeaponClass)
{
	if (InWeaponClass == nullptr) //BP에 할당X시 함수 종료
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this; //스폰 무기는 현재 이 클래스가 소유자 즉,(Player와 NPC)

	AWeapon* SpawnWeapon = GetWorld()->SpawnActor<AWeapon>( //스폰할 액터 타입
		InWeaponClass, //어떤 BP클래스로 스폰?
		GetActorTransform(), // 현재 클래스의 액터의 위치에 스폰 
		SpawnParameters //위 설정 적용
	);

	if (IsValid(SpawnWeapon) == false) //Spawn실패시 함수 종료
	{
		return;
	}
	SpawnWeapon->EquipToCharacter(this); //Weapon클래스의 함수
}
