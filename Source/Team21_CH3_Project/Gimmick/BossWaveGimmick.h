// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossWaveGimmick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectBreakedSignature);

UCLASS()
class TEAM21_CH3_PROJECT_API ABossWaveGimmick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossWaveGimmick();

	void ActivateGimmickObject();

	UPROPERTY(BlueprintAssignable, Category = "Gimmick | Event")
	FOnObjectBreakedSignature OnObjectBreaked;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UParticleSystemComponent* GlowEffectComp;

	UPROPERTY(BlueprintReadOnly, Category = "Gimmick | State")
	bool bIsActivation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick | Settings")
	float ObjectMaxHP;

	UPROPERTY(BlueprintReadOnly, Category = "Gimmick | State")
	float ObjectCurrentHP;
};
