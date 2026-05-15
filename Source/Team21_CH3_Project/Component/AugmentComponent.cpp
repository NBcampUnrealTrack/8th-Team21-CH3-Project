#include "Component/AugmentComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InGameUI/AugmentCardSelectWidget.h"
#include "InGameUI/AugmentCardWidget.h"

UAugmentComponent::UAugmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAugmentComponent::BeginPlay()
{
    Super::BeginPlay();
}


void UAugmentComponent::BindAugmentWidget(UAugmentCardSelectWidget* InWidget)
{
    if (InWidget)
    {
        ActiveWidget = InWidget;

        // 방송을 들을 준비 (바인딩)
        ActiveWidget->OnAugmentSelected.AddDynamic(this, &UAugmentComponent::OnAugmentCardSelected);
    }
}

void UAugmentComponent::OnAugmentCardSelected(FAugmentResult SelectedCardData)
{

    EAugmentType SelectedType = SelectedCardData.Type;

    ApplyAugment(SelectedType);


    if (ActiveWidget)
    {
        ActiveWidget->RemoveFromParent(); // 화면에서 제거
        ActiveWidget = nullptr;           // 참조 해제
    }

}




// 1. 위젯에서 카드 선택 시 호출되는 함수
void UAugmentComponent::ApplyAugment(EAugmentType Type)
{
    // 이미 있으면 레벨업, 없으면 1레벨로 추가
    if (OwnedAugments.Contains(Type))
    {
        OwnedAugments[Type]++;
    }
    else
    {
        OwnedAugments.Add(Type, 1);
    }

    UE_LOG(LogTemp, Warning, TEXT("증강 적용: %d 타입, 현재 레벨: %d"), (uint8)Type, OwnedAugments[Type]);

    // 즉각적인 스탯 변화가 필요한 경우 (예: 위기 본능 등) 여기서 체크 함수 호출 가능
}

// 2. 적 처치 시 로직 (처치 회복, 탄약 순환)
void UAugmentComponent::HandleEnemyKilled(AActor* KilledEnemy)
{
    // 캐릭터 참조 가져오기
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    // [능력 1: 처치 회복]
    if (OwnedAugments.Contains(EAugmentType::HealOnKill))
    {
        const FAugmentTableData* Data = GetAugmentData(EAugmentType::HealOnKill);
        if (Data)
        {
            // 수치 계산: Base + (현재레벨-1) * Upgrade
            // ApplyAugment에서 이미 레벨을 올렸으므로 현재 레벨 기준으로 계산
            int32 Level = OwnedAugments[EAugmentType::HealOnKill];
            float HealAmount = Data->BaseValue + (Level - 1) * Data->UpgradeValue;

            // 캐릭터의 HP를 올리는 함수 호출 (캐릭터에 구현된 함수가 있다고 가정)
            // OwnerChar->AddHealth(HealAmount); 
            UE_LOG(LogTemp, Log, TEXT("처치 회복 발동: %.0f 회복"), HealAmount);
        }
    }

    // [능력 2: 탄약 순환]
    if (OwnedAugments.Contains(EAugmentType::AmmoCycle))
    {
        const FAugmentTableData* Data = GetAugmentData(EAugmentType::AmmoCycle);
        if (Data)
        {
            int32 Level = OwnedAugments[EAugmentType::AmmoCycle];
            float AmmoAmount = Data->BaseValue + (Level - 1) * Data->UpgradeValue;

            // 캐릭터의 탄약을 보충하는 함수 호출
            // OwnerChar->AddAmmo(FMath::FloorToInt(AmmoAmount));
            UE_LOG(LogTemp, Log, TEXT("탄약 순환 발동: %d발 회복"), FMath::FloorToInt(AmmoAmount));
        }
    }
}

// 데이터 테이블에서 수치를 편하게 가져오기 위한 헬퍼 함수
const FAugmentTableData* UAugmentComponent::GetAugmentData(EAugmentType Type)
{
    if (!AugmentDataTable) return nullptr;

    TArray<FAugmentTableData*> AllRows;
    AugmentDataTable->GetAllRows<FAugmentTableData>(TEXT(""), AllRows);

    for (auto Row : AllRows)
    {
        if (Row->Type == Type)
        {
            return Row;
        }
    }
    return nullptr;
}

float UAugmentComponent::GetCalculatedDamage(float InBaseDamage, AActor* Target)
{
    float FinalModifier = 1.0f;

    if (!Target) return InBaseDamage;

    // [능력 3: 보스 사냥꾼]
    if (OwnedAugments.Contains(EAugmentType::BossHunter))
    {
        if (Target->ActorHasTag(TEXT("Boss")))
        {
            const FAugmentTableData* Data = GetAugmentData(EAugmentType::BossHunter);
            if (Data)
            {
                int32 Level = OwnedAugments[EAugmentType::BossHunter];
                float BonusValue = (Data->BaseValue + (Level - 1) * Data->UpgradeValue) / 100.0f;
                FinalModifier += BonusValue;
            }
        }
    }

    // [능력 4: 근거리 압박]
    if (OwnedAugments.Contains(EAugmentType::ClosePressure))
    {
        AActor* OwnerActor = GetOwner();
        if (OwnerActor)
        {
            // 플레이어와 타겟(적) 사이의 거리 계산 (단위: cm / unreal unit)
            float Distance = OwnerActor->GetDistanceTo(Target);

            // 500uu(5미터) 이내인지 확인
            if (Distance <= 500.0f)
            {
                const FAugmentTableData* Data = GetAugmentData(EAugmentType::ClosePressure);
                if (Data)
                {
                    int32 Level = OwnedAugments[EAugmentType::ClosePressure];
                    float BonusValue = (Data->BaseValue + (Level - 1) * Data->UpgradeValue) / 100.0f;
                    FinalModifier += BonusValue;

                    UE_LOG(LogTemp, Log, TEXT("근거리 압박 발동! 거리: %.f, 추가 배율: %.2f"), Distance, BonusValue);
                }
            }
        }
    }

    return InBaseDamage * FinalModifier;
}