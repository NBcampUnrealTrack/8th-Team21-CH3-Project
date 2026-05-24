#include "Item/Weapon.h"
#include "Component/PickupComponent.h"
#include "Character/CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "InGameUI/InGameHUD.h"
#include "Character/PlayerCharacter.h"
#include "Component/AugmentComponent.h"
#include "GameFramework/Character.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
	SetRootComponent(PickupComponent);

	bCanFullAuto = true;
}

void AWeapon::SetCurrentBullsets(int32 InCurrentBullsets)
{
	CurrentBullets = InCurrentBullsets;
}

void AWeapon::EquipToCharacter(ACharacterBase* InCharacter)
{
	FName SocketName;

	switch (WeaponType)
	{
	case EWeaponType::Rifle:
		SocketName = FName(TEXT("hand_rSocket"));
		break;
	case EWeaponType::Shotgun:
		SocketName = FName(TEXT("hand_rSocket_0"));
		break;
	case EWeaponType::Pistol:
		SocketName = FName(TEXT("hand_rSocket_1"));
		break;
	default:
		SocketName = FName(TEXT("hand_rSocket"));
		break;
	}

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//SnapToTarget -> 아이템의 위치,회전,크기를 목표지점과 일치시킴(Snap) /true-> 두 액터를 하나로 붙임
	AttachToComponent(InCharacter->GetMesh(), AttachmentRules, SocketName);
	//아이템 주운 캐릭터의 메시를 타켓으로, 언리얼에서 만들어놓은 소켓에 붙임
	SetActorEnableCollision(false);
	PickupComponent->SetSimulatePhysics(false);
	//아이템에 적용되는 피직스 끔
	InCharacter->CurrentWeapon = this;
	InCharacter->CurrentWeaponType = WeaponType;

	APlayerCharacter* Player = Cast<APlayerCharacter>(InCharacter);
	if (IsValid(Player))
	{
		Player->FirePerMinute = FirePerMinute;
		OnAmmoChanged.AddUObject(Player, &APlayerCharacter::OnAmmoChanged);
	}

	APlayerController* PlayerController = Cast<APlayerController>(InCharacter->GetController());
	if (IsValid(PlayerController))
	{
		AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
		if (IsValid(InGameHUD))
		{
			// 무기를 장착하면 현재 탄약 정보를 UI에 즉시 반영한다.
			InGameHUD->RefreshAmmoUI(CurrentBullets, MaxBullets);
		}
	}
}

void AWeapon::Reload()
{
	CurrentBullets = MaxBullets;
	OnAmmoChanged.Broadcast(CurrentBullets, MaxBullets);
	//UE_LOG(LogTemp, Warning, TEXT("[리로딩 완료] %d / %d"), CurrentBullets, MaxBullets);

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar)
	{
		UAugmentComponent* AugmentComp = OwnerChar->FindComponentByClass<UAugmentComponent>();

		if (AugmentComp)
		{
			AugmentComp->HandleReloadFinished();
		}
	}
}

bool AWeapon::UseBullets()
{
	if (0 >= CurrentBullets)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ReLoading!!"));
			return false;
	}

	CurrentBullets--;
	OnAmmoChanged.Broadcast(CurrentBullets, MaxBullets);
	//UE_LOG(LogTemp, Warning, TEXT("Bullets: %d / %d"), CurrentBullets, MaxBullets);

	return true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	PickupComponent->OnPickUp.AddDynamic(this, &ThisClass::HandleOnPickUp);
}

void AWeapon::HandleOnPickUp(ACharacterBase* InPickUpCharacter)
{
	if (IsValid(InPickUpCharacter) == false)
	{
		return;
	}

	EquipToCharacter(InPickUpCharacter);
}