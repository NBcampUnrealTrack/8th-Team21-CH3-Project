// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ammo.generated.h"

class UPickupComponent;
class ACharacterBase;

UCLASS()
class TEAM21_CH3_PROJECT_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmo();

	UPickupComponent* GetPickupComponent() const{ return Pickupcomponent; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnPickUp(ACharacterBase* InPickUpCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void AmmoOnPickUp();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPickupComponent> Pickupcomponent;

};
