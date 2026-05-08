
#include "Door.h"
#include "TimerManager.h"
#include "Character/PlayerCharacter.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;


	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;


	Doorcomponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	Doorcomponent->SetupAttachment(SceneRoot);


	Doorcomponent->SetCollisionProfileName(TEXT("BlockAll"));

}

void ADoor::Interact(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("Door Interaction Started!"));

	TargetRelativeLocation = OpenRelativeLocation;

	GetWorldTimerManager().ClearTimer(TimerHandle_Return);

	GetWorldTimerManager().SetTimer(TimerHandle_Return, this, &ADoor::CloseDoor, ReturnDelay, false);
		
		
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedRelativeLocation = Doorcomponent->GetRelativeLocation();

	OpenRelativeLocation = ClosedRelativeLocation + FVector(0.f, OpenDistance, 0.f);
	
	TargetRelativeLocation = ClosedRelativeLocation;
	
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = Doorcomponent->GetRelativeLocation();

	if (!CurrentLocation.Equals(TargetRelativeLocation, 0.5f))
	{
		FVector NextLocation = FMath::VInterpTo(CurrentLocation, TargetRelativeLocation, DeltaTime, MoveSpeed);
		Doorcomponent->SetRelativeLocation(NextLocation);
	}

}


void ADoor::CloseDoor()
{
	UE_LOG(LogTemp, Log, TEXT("Door Closing Automatically..."));

	TargetRelativeLocation = ClosedRelativeLocation;
}