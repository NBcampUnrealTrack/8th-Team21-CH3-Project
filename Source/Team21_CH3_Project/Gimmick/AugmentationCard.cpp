#include "Gimmick/AugmentationCard.h"
#include "InGameUI/AugmentCardWidget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Data/AugmentationDataTable.h"
#include "Component/AugmentComponent.h"

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

    // 1. 증강 컴포넌트가 있는 액터(플레이어)인지 확인
    UAugmentComponent* AugmentComp = OtherActor->FindComponentByClass<UAugmentComponent>();
    if (!AugmentComp || !AugmentDataTable || !AugmentWidgetClass) return;

    // 2. 랜덤 셔플
    TArray<FName> RowNames = AugmentDataTable->GetRowNames();
    if (RowNames.Num() < 3) return;

    for (int32 i = RowNames.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        RowNames.Swap(i, j);
    }

    // 3. 선택지 생성
    TArray<FAugmentResult> FinalOptions;
    for (int32 i = 0; i < 3; i++)
    {
        FAugmentTableData* Row = AugmentDataTable->FindRow<FAugmentTableData>(RowNames[i], TEXT(""));
        if (Row)
        {
            FAugmentResult Option;
            Option.Type = Row->Type;

            // 컴포넌트에서 현재 상태를 읽어옴
            int32 CurrentLevel = AugmentComp->GetAugmentLevel(Row->Type);
            Option.CurrentLevel = CurrentLevel + 1;
            Option.DisplayTitle = FString::Printf(TEXT("%s (Lv.%d)"), *Row->AugmentName, Option.CurrentLevel);

            // 수치 계산 (중첩 횟수만큼 가산)
            float DisplayValue = Row->BaseValue + (CurrentLevel * Row->UpgradeValue);

            // 설명문 조립
            FFormatNamedArguments Args;
            Args.Add(TEXT("Value"), FText::AsNumber(FMath::FloorToInt(DisplayValue)));
            Args.Add(TEXT("Unit"), FText::FromString(Row->UnitText));
            Option.Description = FText::Format(FText::FromString(Row->DescriptionFormat), Args).ToString();

            FinalOptions.Add(Option);
        }
    }
    /*
    // 4. 위젯 생성 및 출력
    if (UAugmentWidget* WidgetInstance = CreateWidget<UAugmentWidget>(GetWorld(), AugmentWidgetClass))
    {
        WidgetInstance->OnDataReceived(FinalOptions);
        WidgetInstance->AddToViewport();

        // 컨트롤러 설정 로직 (기존과 동일)
        if (APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
        {
            PC->SetPause(true);
            PC->bShowMouseCursor = true;
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(WidgetInstance->TakeWidget());
            PC->SetInputMode(InputMode);
        }
    }
    */
    Destroy(); // 카드 제거
}