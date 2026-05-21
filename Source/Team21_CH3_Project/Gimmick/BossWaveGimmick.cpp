#include "Gimmick/BossWaveGimmick.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"


ABossWaveGimmick::ABossWaveGimmick()
{

	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;

	GlowEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlowEffect"));
	GlowEffectComp->SetupAttachment(RootComponent);

	bIsActivation = false;
	ObjectMaxHP = 150.0f;
	ObjectCurrentHP = ObjectMaxHP;

}


void ABossWaveGimmick::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GlowEffectComp->SetVisibility(false);
	
}


void ABossWaveGimmick::ActivateGimmickObject()
{
	if (bIsActivation) return;

	bIsActivation = true;
	ObjectCurrentHP = ObjectMaxHP;

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GlowEffectComp->SetVisibility(true);

	UE_LOG(LogTemp, Warning, TEXT("BossWaveGimmick Object Activated!"));
}


float ABossWaveGimmick::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("!!! TakeDamage Called !!! Damage: %f, bIsActivation: %d"), DamageAmount, bIsActivation);
	if (!bIsActivation) return 0.0f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ObjectCurrentHP = FMath::Clamp(ObjectCurrentHP - ActualDamage, 0.0f, ObjectMaxHP);

	UE_LOG(LogTemp, Log, TEXT("Gimmick Object HP: %.1f / %.1f"), ObjectCurrentHP, ObjectMaxHP);

	if (ObjectCurrentHP <= 0.0f)
	{
		bIsActivation = false;

		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GlowEffectComp->SetVisibility(false);

		MeshComp->SetVisibility(false);

		if (OnObjectBreaked.IsBound())
		{
			OnObjectBreaked.Broadcast();
		}
	}

	return ActualDamage;
}