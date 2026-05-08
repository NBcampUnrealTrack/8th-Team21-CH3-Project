
#include "Gimmick/ItemBox.h"
#include "Character/PlayerCharacter.h"

AItemBox::AItemBox()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Boxcomponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(Boxcomponent);
	Boxcomponent->SetCollisionProfileName(TEXT("BlockAll"));

}

void AItemBox::Interact(AActor* Interactor)
{
	if (bIsOpened) return;

	APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
	if (Player)
	{
		UE_LOG(LogTemp, Log, TEXT("Box Opened!"));

		int32 LootCount = FMath::RandRange(1, 5);
		for (int32 i = 0; i < LootCount; i++)
		{
			GiveRandomItem(Player);
		}

		bIsOpened = true;
		
		SetLifeSpan(2.0f);
	}


}

void AItemBox::GiveRandomItem(AActor* TargetActor)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(TargetActor);
	if (!Player) return;

	float Chance = FMath::FRand();

	if (Chance < 0.5f)
	{
		UE_LOG(LogTemp, Log, TEXT("Bandage Given!"));
	}
	else if (Chance < 0.8f)
	{
		UE_LOG(LogTemp, Log, TEXT("Ammo Given!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("HealthPack Given!"));
	}

	
}

