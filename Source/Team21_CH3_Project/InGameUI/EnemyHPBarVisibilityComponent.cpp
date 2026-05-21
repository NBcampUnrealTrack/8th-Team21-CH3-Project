// EnemyHPBarVisibilityComponent.cpp

#include "EnemyHPBarVisibilityComponent.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UEnemyHPBarVisibilityComponent::UEnemyHPBarVisibilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxVisibleDistance = 25000.0f;
	TargetHeightOffset = 120.0f;

	HPBarComponentName = TEXT("HPBar");
	HPBarComponentTag = TEXT("HPBar");

	TraceChannel = ECC_Visibility;

	HPBarWidgetComponent = nullptr;
}

void UEnemyHPBarVisibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheHPBarWidgetComponent();
	SetHPBarVisible(false);
}

void UEnemyHPBarVisibilityComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHPBarVisibility();
}

void UEnemyHPBarVisibilityComponent::CacheHPBarWidgetComponent()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TArray<UWidgetComponent*> WidgetComponents;
	Owner->GetComponents<UWidgetComponent>(WidgetComponents);

	for (UWidgetComponent* WidgetComponent : WidgetComponents)
	{
		if (!WidgetComponent)
		{
			continue;
		}

		if (WidgetComponent->GetFName() == HPBarComponentName || WidgetComponent->ComponentHasTag(HPBarComponentTag))
		{
			HPBarWidgetComponent = WidgetComponent;
			return;
		}
	}

	if (WidgetComponents.Num() > 0)
	{
		HPBarWidgetComponent = WidgetComponents[0];
	}
}

void UEnemyHPBarVisibilityComponent::UpdateHPBarVisibility()
{
	if (!HPBarWidgetComponent)
	{
		CacheHPBarWidgetComponent();

		if (!HPBarWidgetComponent)
		{
			return;
		}
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		SetHPBarVisible(false);
		return;
	}

	const FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	const FVector CameraForward = CameraRotation.Vector();

	const bool bShouldShow =
		IsWithinVisibleDistance(CameraLocation) &&
		IsInFrontOfCamera(CameraLocation, CameraForward) &&
		HasLineOfSightFromCamera(CameraLocation);

	SetHPBarVisible(bShouldShow);
}

bool UEnemyHPBarVisibilityComponent::IsWithinVisibleDistance(const FVector& CameraLocation) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	const float DistanceSquared = FVector::DistSquared(CameraLocation, Owner->GetActorLocation());
	return DistanceSquared <= FMath::Square(MaxVisibleDistance);
}

bool UEnemyHPBarVisibilityComponent::IsInFrontOfCamera(
	const FVector& CameraLocation,
	const FVector& CameraForward
) const
{
	const FVector TargetLocation = GetTargetLocation();
	const FVector DirectionToTarget = (TargetLocation - CameraLocation).GetSafeNormal();

	const float Dot = FVector::DotProduct(CameraForward, DirectionToTarget);

	return Dot > 0.0f;
}

bool UEnemyHPBarVisibilityComponent::HasLineOfSightFromCamera(const FVector& CameraLocation) const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = PlayerController ? PlayerController->GetPawn() : nullptr;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlayerPawn);
	QueryParams.bTraceComplex = false;

	FHitResult HitResult;
	const FVector Start = CameraLocation;
	const FVector End = GetTargetLocation();

	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		TraceChannel,
		QueryParams
	);

	if (!bHit)
	{
		return true;
	}

	AActor* HitActor = HitResult.GetActor();

	if (HitActor == Owner)
	{
		return true;
	}

	if (HitActor && HitActor->IsAttachedTo(Owner))
	{
		return true;
	}

	return false;
}

FVector UEnemyHPBarVisibilityComponent::GetTargetLocation() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	return Owner->GetActorLocation() + FVector(0.0f, 0.0f, TargetHeightOffset);
}

void UEnemyHPBarVisibilityComponent::SetHPBarVisible(bool bVisible)
{
	if (!HPBarWidgetComponent)
	{
		return;
	}

	if (HPBarWidgetComponent->IsVisible() == bVisible)
	{
		return;
	}

	HPBarWidgetComponent->SetVisibility(bVisible, true);
}