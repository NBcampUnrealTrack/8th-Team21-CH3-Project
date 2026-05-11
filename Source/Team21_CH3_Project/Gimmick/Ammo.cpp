
#include "Gimmick/Ammo.h"
#include "Component/PickupComponent.h"
#include "Character/CharacterBase.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = false;

	Pickupcomponent = CreateDefaultSubobject<UPickupComponent>(TEXT("Pickupcomponent"));
	SetRootComponent(Pickupcomponent);

}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	
	Pickupcomponent->OnPickUp.AddDynamic(this, &ThisClass::HandleOnPickUp);
}

void AAmmo::HandleOnPickUp(ACharacterBase* InPickUpCharacter)
{
	if (IsValid(InPickUpCharacter) == false)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Ammo Picked Up!"));
	AmmoOnPickUp();

	Pickupcomponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
}




