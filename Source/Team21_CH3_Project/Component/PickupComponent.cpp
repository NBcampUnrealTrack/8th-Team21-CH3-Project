// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PickupComponent.h"
#include "Character/PlayerCharacter.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOnComponentBeginOverlap);
}

void UPickupComponent::HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* OverlappedCharacter = Cast<ACharacterBase>(OtherActor);
	if (IsValid(OverlappedCharacter) == true)
	{
		OnPickUp.Broadcast(OverlappedCharacter);

		OnComponentBeginOverlap.RemoveAll(this);
	}
}
