// HPBarVisComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HPBarVisComponent.generated.h"

class UWidgetComponent;
class UTeamHPTextWidgetComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM21_CH3_PROJECT_API UHPBarVisComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHPBarVisComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP Bar Visibility")
	float MaxVisibleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP Bar Visibility")
	float TargetHeightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP Bar Visibility")
	FName HPBarComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP Bar Visibility")
	FName HPBarComponentTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HP Bar Visibility")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY()
	UWidgetComponent* HPBarWidgetComponent;

protected:
	void CacheHPBarWidgetComponent();
	void UpdateHPBarVisibility();

	bool IsWithinVisibleDistance(const FVector& CameraLocation) const;
	bool IsInFrontOfCamera(const FVector& CameraLocation, const FVector& CameraForward) const;
	bool HasLineOfSightFromCamera(const FVector& CameraLocation) const;

	FVector GetTargetLocation() const;
	void SetHPBarVisible(bool bVisible);
};