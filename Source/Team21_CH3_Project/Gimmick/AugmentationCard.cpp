
#include "Gimmick/AugmentationCard.h"
//#include "AugmentWidget.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

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
    // 캐릭터(Player)인지 확인
    if (OtherActor && OtherActor != this && OtherActor->IsA(APawn::StaticClass()))
    {
        if (!AugmentDataTable || !AugmentWidgetClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Data Table or Widget Class is missing!"));
            return;
        }

        // 1. 데이터 테이블에서 모든 행 이름 가져오기
        TArray<FName> RowNames = AugmentDataTable->GetRowNames();
        if (RowNames.Num() == 0) return;

        TArray<FAugmentResult> FinalOptions;

        // 2. 3회 랜덤 추출 진행 (중복 허용)
        for (int32 i = 0; i < 3; i++)
        {
            int32 RandIdx = FMath::RandRange(0, RowNames.Num() - 1);
            FAugmentTableData* Row = AugmentDataTable->FindRow<FAugmentTableData>(RowNames[RandIdx], TEXT(""));

            if (Row)
            {
                FAugmentResult Option;
                Option.Type = Row->Type;

                // 수치 랜덤 결정 (Min ~ Max 사이) 후 소수점 버림
                float RolledValue = FMath::FRandRange(Row->MinValue, Row->MaxValue);
                Option.FinalValue = FMath::FloorToFloat(RolledValue);

                // UI에 띄울 텍스트 조립
                Option.DisplayTitle = Row->StatName + TEXT(" 강화");
                Option.Description = FString::Printf(TEXT("%s %.0f%s"), *Row->StatName, Option.FinalValue, *Row->UnitText);

                FinalOptions.Add(Option);
            }
        }

        // 3. UI 위젯 생성 및 데이터 전달
        /*UAugmentWidget* WidgetInstance = CreateWidget<UAugmentWidget>(GetWorld(), AugmentWidgetClass);
        if (WidgetInstance)
        {
            // 위젯에 데이터를 넘겨주는 함수 (위젯 클래스에 구현 예정)
            WidgetInstance->OnDataReceived(FinalOptions);
            WidgetInstance->AddToViewport();

            // 4. 게임 정지 및 마우스 커서 설정
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC)
            {
                PC->SetPause(true);
                PC->bShowMouseCursor = true;

                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(WidgetInstance->TakeWidget());
                PC->SetInputMode(InputMode);
            }
        }
        */
        // 5. 카드는 할 일을 다 했으니 월드에서 제거
        Destroy();
    }
}

