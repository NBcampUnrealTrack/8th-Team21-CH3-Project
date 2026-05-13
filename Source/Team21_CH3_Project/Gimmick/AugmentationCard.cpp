#include "Gimmick/AugmentationCard.h"
//#include "AugmentWidget.h" // 주석 해제 (위젯 클래스 헤더)
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Data/AugmentationDataTable.h"

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
    if (OtherActor && OtherActor != this && OtherActor->IsA(APawn::StaticClass()))
    {
        if (!AugmentDataTable || !AugmentWidgetClass) return;

        // 1. 전체 데이터 행 이름 가져오기
        TArray<FName> RowNames = AugmentDataTable->GetRowNames();
        if (RowNames.Num() < 3) return;

        // 2. 셔플(Shuffle)을 이용한 중복 없는 추출
        for (int32 i = RowNames.Num() - 1; i > 0; i--)
        {
            int32 j = FMath::RandRange(0, i);
            RowNames.Swap(i, j);
        }

        TArray<FAugmentResult> FinalOptions;

        // TODO: 플레이어 상태(예: PlayerState)에서 현재 소유한 증강 정보를 가져오는 로직이 필요합니다.
        // 여기서는 예시로 로컬 변수를 사용하지만, 실제로는 플레이어 클래스에서 데이터를 참조하세요.

        for (int32 i = 0; i < 3; i++)
        {
            FAugmentTableData* Row = AugmentDataTable->FindRow<FAugmentTableData>(RowNames[i], TEXT(""));
            if (Row)
            {
                FAugmentResult Option;
                Option.Type = Row->Type;

                // [중첩 로직] 플레이어가 이미 가지고 있는 레벨을 확인 (임시로 0 설정)
                int32 CurrentLevel = 0;
                int32 NextLevel = CurrentLevel + 1;

                // 수치 계산: 기본값 + (중첩 횟수 * 상승값)
                float CalculatedValue = Row->BaseValue + (CurrentLevel * Row->UpgradeValue);

                Option.CurrentLevel = NextLevel;
                Option.DisplayTitle = FString::Printf(TEXT("%s (Lv.%d)"), *Row->AugmentName, NextLevel);

                // [에러 해결] FText::Format을 사용하여 변수 기반의 설명문 조립
                // 데이터 테이블의 DescriptionFormat 예시: "{Value}{Unit} 회복"
                FFormatNamedArguments Args;
                Args.Add(TEXT("Value"), FText::AsNumber(FMath::FloorToInt(CalculatedValue)));
                Args.Add(TEXT("Unit"), FText::FromString(Row->UnitText));

                // DescriptionFormat이 FString일 경우 FText로 변환하여 포맷팅
                Option.Description = FText::Format(FText::FromString(Row->DescriptionFormat), Args).ToString();

                FinalOptions.Add(Option);
            }
        }

        // 3. UI 위젯 생성 및 데이터 전달
        /*UAugmentWidget* WidgetInstance = CreateWidget<UAugmentWidget>(GetWorld(), AugmentWidgetClass);
        if (WidgetInstance)
        {
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
        // 5. 카드는 월드에서 제거
        Destroy();
    }
}