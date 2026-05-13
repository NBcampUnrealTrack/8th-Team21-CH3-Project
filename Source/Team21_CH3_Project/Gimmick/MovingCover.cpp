// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/MovingCover.h"

// Sets default values
AMovingCover::AMovingCover()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMovingCover::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	
}

// Called every frame
void AMovingCover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;
    if (ElapsedTime < StartDelay)
    {
        return;
    }

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = bIsMovingDown ? (InitialLocation - FVector(0, 0, MoveDistance)) : InitialLocation;

    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);
    SetActorLocation(NewLocation);

    if (FVector::Dist(NewLocation, TargetLocation) < 1.0f)
    {
        bIsMovingDown = !bIsMovingDown;
    }

}

