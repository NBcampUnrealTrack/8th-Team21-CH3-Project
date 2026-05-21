// BossMeteorStrikeActor.h
#include "BossMeteorStrikeActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Components/AudioComponent.h"

ABossMeteorStrikeActor::ABossMeteorStrikeActor(){
	
	// 1. Target location save
	// 2. warningCircle mark
	// 3. meteorMesh falling
	// 4. Timer is End = Damage apply
	// 5. Explosion effect, sound play
	// 6. Destroy actor
	
	sceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(sceneRoot);
	
	meteorNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MeteorNiagaraComponent"));
	meteorNiagara->SetupAttachment(sceneRoot);
	
	warningCircle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningCircleComponent"));
	warningCircle->SetupAttachment(sceneRoot);
	
	fillWarningCircleDuration = 2.0f;
	meteorStartHeight = 1500.0f;

	damage = 30.0f;
	warningStartScale = FVector(0.01f, 0.01f, 1.0f);
	warningTargetScale = FVector(6.0f, 6.0f, 1.0f);
	warningRelativeOffset = FVector::ZeroVector;
	meteorStartOffset = FVector(0.0f, -3.0f, meteorStartHeight);
	meteorEndOffset = FVector::ZeroVector;
	elapsedTime = 0.0f;
	
	
	PrimaryActorTick.bCanEverTick = true;
}

void ABossMeteorStrikeActor::BeginPlay(){
	Super::BeginPlay();
	
	impactRadius = warningTargetScale.X * 100.0f * 0.5f;
	
	if (IsValid(fallingSound) == true)
	{
		fallingAudioComponent = UGameplayStatics::SpawnSoundAtLocation(
			this,
			fallingSound,
			GetActorLocation());
	}
	
	warningCircle->SetRelativeLocation(warningRelativeOffset);
	warningCircle->SetRelativeScale3D(warningStartScale);
	
	if (IsValid(meteorNiagara) == true)
	{
		meteorNiagara->SetRelativeLocation(meteorStartOffset);
		meteorNiagara->Activate(true);
	}
}

void ABossMeteorStrikeActor::Tick(float DeltaSeconds){
	elapsedTime += DeltaSeconds;
	
	float alpha = 0.0f;
	alpha = FMath::Clamp(elapsedTime / fillWarningCircleDuration, 0.0f, 1.0f);
	
	const FVector NewLocation = FMath::Lerp(meteorStartOffset, meteorEndOffset, alpha);
	const FVector warningCircleScale = FMath::Lerp(warningStartScale, warningTargetScale, alpha);
	
	warningCircle->SetRelativeScale3D(warningCircleScale);
	if (IsValid(meteorNiagara) == true)
	{
		meteorNiagara->SetRelativeLocation(NewLocation);
	}
	
	if (alpha >= 1)
	{
		if (IsValid(impactNiagaraSystem) == true)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			impactNiagaraSystem,
				GetActorLocation()
			);	
		}
		
		if (IsValid(fallingAudioComponent) == true) fallingAudioComponent->Stop();
		
		if (IsValid(impactSound) == true)
		{
			UGameplayStatics::PlaySoundAtLocation(this, impactSound, GetActorLocation());
		}
		
		ApplyDamageInRadius();
		
		Destroy();
	}
}

void ABossMeteorStrikeActor::ApplyDamageInRadius(){
	APawn* playerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(playerPawn) == false) return;
	
	const float distance = FVector::Dist2D(GetActorLocation(), playerPawn->GetActorLocation());
	if (distance > impactRadius) return;
	
	FDamageEvent damageEvent;
	playerPawn->TakeDamage(damage, damageEvent, nullptr, this);
}
