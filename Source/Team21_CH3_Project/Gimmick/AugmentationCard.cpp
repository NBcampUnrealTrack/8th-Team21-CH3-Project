#include "Gimmick/AugmentationCard.h"
#include "InGameUI/AugmentCardWidget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Data/AugmentationDataTable.h"
#include "Component/AugmentComponent.h"
#include "InGameUI/AugmentCardSelectWidget.h"

AAugmentCard::AAugmentCard()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    SetRootComponent(CollisionBox);
    CollisionBox->SetCollisionProfileName(TEXT("Trigger"));

    CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    CardMesh->SetupAttachment(RootComponent);
    CardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAugmentCard::OnOverlapBegin);
}


void AAugmentCard::BeginPlay()
{
    Super::BeginPlay();
}

void AAugmentCard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    UAugmentComponent* AugmentComp = OtherActor->FindComponentByClass<UAugmentComponent>();
    if (!AugmentComp || !AugmentDataTable || !AugmentWidgetClass) return;
    
    AugmentSelection(AugmentComp);

    Destroy();
}


void AAugmentCard::AugmentSelection(UAugmentComponent* AugmentComp)
{
    if (!AugmentDataTable || !AugmentWidgetClass) return;


    TArray<FName> RowNames = AugmentDataTable->GetRowNames();
    if (RowNames.Num() < 3) return;

    for (int32 i = RowNames.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        RowNames.Swap(i, j);
    }


    TArray<FAugmentResult> FinalOptions;
    for (int32 i = 0; i < 3; i++)
    {
        FAugmentTableData* Row = AugmentDataTable->FindRow<FAugmentTableData>(RowNames[i], TEXT(""));
        if (Row)
        {
            FAugmentResult Option;
            Option.Type = Row->Type;


            int32 CurrentLevel = AugmentComp->GetAugmentLevel(Row->Type);
            Option.CurrentLevel = CurrentLevel + 1;
            Option.DisplayTitle = FString::Printf(TEXT("%s (Lv.%d)"), *Row->AugmentName, Option.CurrentLevel);


            float DisplayValue = Row->BaseValue + (CurrentLevel * Row->UpgradeValue);


            FFormatNamedArguments Args;
            Args.Add(TEXT("Value"), FText::AsNumber(FMath::FloorToInt(DisplayValue)));
            Args.Add(TEXT("Unit"), FText::FromString(Row->UnitText));
            Option.Description = FText::Format(FText::FromString(Row->DescriptionFormat), Args).ToString();

            FinalOptions.Add(Option);
        }
    }


    if (UAugmentCardSelectWidget* WidgetInstance = CreateWidget<UAugmentCardSelectWidget>(GetWorld(), AugmentWidgetClass))
    {
        WidgetInstance->OnDataReceived(FinalOptions);
        WidgetInstance->AddToViewport();

        AugmentComp->BindAugmentWidget(WidgetInstance);

        if (APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
        {
            PC->SetPause(true);
            PC->bShowMouseCursor = true;
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(WidgetInstance->TakeWidget());
            PC->SetInputMode(InputMode);
        }
    }
}