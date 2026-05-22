#include "Gimmick/BossWaveGimmick.h"
#include "Character/CharacterBase.h"
#include "Component/StatusComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/NonPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


ABossWaveGimmick::ABossWaveGimmick()
{

	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	GlowEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlowEffect"));
	GlowEffectComp->SetupAttachment(RootComponent);

	bIsActivation = false;
	ObjectMaxHP = 200.0f;
	ObjectCurrentHP = ObjectMaxHP;

}



void ABossWaveGimmick::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetVisibility(false);
	GlowEffectComp->SetVisibility(false);

}

void ABossWaveGimmick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsValid(TargetBoss))
	{
		TArray<AActor*> FoundBosses;
		UGameplayStatics::GetAllActorsOfClass(this, ANonPlayerCharacter::StaticClass(), FoundBosses);

		for (AActor* Actor : FoundBosses)
		{
			if (Actor && Actor->ActorHasTag(TEXT("Boss")))
			{
				TargetBoss = Cast<ACharacterBase>(Actor);
				if (IsValid(TargetBoss))
				{
					UE_LOG(LogTemp, Error, TEXT("[GIMMICK SELF-SCAN SUCCESS] Found Boss via World Scan: %s"), *TargetBoss->GetName());
					break;
				}
			}
		}
	}

	if (bIsActivation && IsValid(TargetBoss))
	{
		UStatusComponent* BossStatus = GetStatus();
		if (IsValid(BossStatus))
		{
			float HealAmount = 10.f * DeltaSeconds;
			float NewHP = FMath::Clamp(BossStatus->GetCurrentHP() + HealAmount, 0.f, BossStatus->GetMaxHP());
			BossStatus->SetCurrentHP(NewHP);
		}
	}
}


void ABossWaveGimmick::ActivateGimmickObject()
{
	if (bHasBeenActivated || bIsActivation) return;

	bIsActivation = true;
	bHasBeenActivated = true;
	ObjectCurrentHP = ObjectMaxHP;

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetVisibility(true);
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

void ABossWaveGimmick::SetTargetBoss(ACharacterBase* NewBoss)
{
	if (IsValid(NewBoss))
	{
		TargetBoss = NewBoss;
		UE_LOG(LogTemp, Log, TEXT("[Gimmick] TargetBoss is Successed: %s"), *TargetBoss->GetName());
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[!!! ERROR !!!] SetTargetBoss called, but NewBoss parameter is NULL!"));
	}
}

UStatusComponent* ABossWaveGimmick::GetStatus() {
	ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(TargetBoss);

	if (IsValid(Boss) == false) return nullptr;

	UStatusComponent* status = Boss->GetStatusComponent();
	if (IsValid(status) == false) return nullptr;

	return status;
}
