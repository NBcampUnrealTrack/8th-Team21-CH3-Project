// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingCover.generated.h"

UCLASS()
class TEAM21_CH3_PROJECT_API AMovingCover : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingCover();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	FVector InitialLocation;

	bool bIsMovingDown = true;
	float ElapsedTime = 0.0f;


	UPROPERTY(EditAnywhere, Category="Cover")
	float MoveDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category="Cover")
	float MoveSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category="Cover")
	float StartDelay = 2.0f;


};
