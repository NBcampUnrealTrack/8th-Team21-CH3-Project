
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"

#include "AugmentationCard.generated.h"


class UAugmentWidget; 
class UAugmentComponent;

UCLASS()
class TEAM21_CH3_PROJECT_API AAugmentCard : public AActor
{
    GENERATED_BODY()

public:
    AAugmentCard();

    

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UBoxComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UStaticMeshComponent* CardMesh;

    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* AugmentDataTable;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> AugmentWidgetClass;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    void AugmentSelection(UAugmentComponent* InAugmentComp);
};
