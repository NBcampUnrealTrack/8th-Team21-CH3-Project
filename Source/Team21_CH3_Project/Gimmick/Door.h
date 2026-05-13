// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interaction.h"
#include "Door.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API ADoor : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

	virtual void Interact(AActor* Interactor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* Doorcomponent;

	UPROPERTY(EditAnywhere, Category = "DoorSettings")
	float OpenDistance = 120.0f;

	UPROPERTY(EditAnywhere, Category = "DoorSettings")
	float ReturnDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "DoorSettings")
	float MoveSpeed = 3.0f;

public:	

	virtual void Tick(float DeltaTime) override;

private:
	FVector ClosedRelativeLocation;
	FVector OpenRelativeLocation;
	FVector TargetRelativeLocation;

	FTimerHandle TimerHandle_Return;
	bool bIsMoving = false;

	void CloseDoor();

};
