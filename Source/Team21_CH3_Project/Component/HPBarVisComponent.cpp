// HPBarVisComponent.cpp

#include "Component/HPBarVisComponent.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"
#include "InGameUI/TeamHPTextWidgetComponent.h"
#include "Kismet/GameplayStatics.h"

UHPBarVisComponent::UHPBarVisComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxVisibleDistance = 25000.0f;
	TargetHeightOffset = 120.0f;

	HPBarComponentName = TEXT("HPBar");
	HPBarComponentTag = TEXT("HPBar");

	TraceChannel = ECC_Visibility;

	HPBarWidgetComponent = nullptr;
}

void UHPBarVisComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheHPBarWidgetComponent();
	SetHPBarVisible(false);
}

void UHPBarVisComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHPBarVisibility();
}

void UHPBarVisComponent::CacheHPBarWidgetComponent()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UTeamHPTextWidgetComponent* TeamHPTextWidgetComponent = Owner->FindComponentByClass<UTeamHPTextWidgetComponent>();
	if (TeamHPTextWidgetComponent)
	{
		HPBarWidgetComponent = TeamHPTextWidgetComponent;
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

void UHPBarVisComponent::UpdateHPBarVisibility()
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

bool UHPBarVisComponent::IsWithinVisibleDistance(const FVector& CameraLocation) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	const float DistanceSquared = FVector::DistSquared(CameraLocation, Owner->GetActorLocation());
	return DistanceSquared <= FMath::Square(MaxVisibleDistance);
}

bool UHPBarVisComponent::IsInFrontOfCamera(
	const FVector& CameraLocation,
	const FVector& CameraForward
) const
{
	const FVector TargetLocation = GetTargetLocation();
	const FVector DirectionToTarget = (TargetLocation - CameraLocation).GetSafeNormal();

	const float Dot = FVector::DotProduct(CameraForward, DirectionToTarget);

	return Dot > 0.0f;
}

bool UHPBarVisComponent::HasLineOfSightFromCamera(const FVector& CameraLocation) const
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
	QueryParams.AddIgnoredActor(Owner);
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

	return !bHit;
}

FVector UHPBarVisComponent::GetTargetLocation() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	return Owner->GetActorLocation() + FVector(0.0f, 0.0f, TargetHeightOffset);
}

void UHPBarVisComponent::SetHPBarVisible(bool bVisible)
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