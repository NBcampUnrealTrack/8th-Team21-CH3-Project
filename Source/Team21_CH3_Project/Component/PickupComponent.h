
#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PickupComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, ACharacterBase*, InPickUpCharacter);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM21_CH3_PROJECT_API UPickupComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:

	UPickupComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(BlueprintAssignable)
	FOnPickUp OnPickUp;
};
