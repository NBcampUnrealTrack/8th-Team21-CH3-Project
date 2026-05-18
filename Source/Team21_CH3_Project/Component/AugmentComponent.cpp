#include "Component/AugmentComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InGameUI/AugmentCardSelectWidget.h"
#include "InGameUI/AugmentCardWidget.h"
#include "InGameUI/ShooterInGameMode.h"
#include "Component/StatusComponent.h"
#include "Character/PlayerCharacter.h"
#include "Item/Weapon.h"
#include "Character/CharacterBase.h"

UAugmentComponent::UAugmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("/Script/Engine.DataTable'/Game/ShooterX/Gimmick/DataTable/DT_Agumentations.DT_Agumentations'"));

    if (DataTableAsset.Succeeded())
    {
        AugmentDataTable = DataTableAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAugmentComponent: Failed to load DataTable from Path!"));
    }
}

void UAugmentComponent::BeginPlay()
{
    Super::BeginPlay();

    AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld());
    if (GM)
    {
  
        AShooterInGameMode* MyGM = Cast<AShooterInGameMode>(GM);
        if (MyGM)
        {
            MyGM->OnEnemyKilledDelegate.AddDynamic(this, &UAugmentComponent::HandleEnemyKilled);
        }
    }

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (OwnerChar)
    {
        UStatusComponent* StatusComp = OwnerChar->FindComponentByClass<UStatusComponent>();
        if (StatusComp)
        {
            StatusComp->OnCurrentHPChanged.AddUObject(this, &UAugmentComponent::CheckLowHPSpeedBuff);
        }
    }
}


void UAugmentComponent::ApplyAugment(EAugmentType Type)
{

    if (OwnedAugments.Contains(Type))
    {
        OwnedAugments[Type]++;
    }
    else
    {
        OwnedAugments.Add(Type, 1);
    }

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

    ApplyAugment(SelectedCardData.Type);


    if (IsValid(ActiveWidget))
    {
        ActiveWidget->RemoveFromParent();
        ActiveWidget = nullptr;
    }


    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
        if (PC)
        {
            PC->SetPause(false);
            PC->bShowMouseCursor = false;
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }

}


void UAugmentComponent::HandleEnemyKilled(AActor* KilledEnemy)
{
    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar) return;

    int32 TotalAugments = OwnedAugments.Num();

    if (OwnedAugments.Contains(EAugmentType::HealOnKill))
    {
        const FAugmentTableData* Data = GetAugmentData(EAugmentType::HealOnKill);
        if (Data)
        {
            int32 Level = OwnedAugments[EAugmentType::HealOnKill];
            float HealAmount = Data->BaseValue + (Level - 1) * Data->UpgradeValue;
            UStatusComponent* StatusComp = OwnerChar->FindComponentByClass<UStatusComponent>();
            if (StatusComp)
            {
                float CurrentHP = StatusComp->GetCurrentHP();
                float MaxHP = StatusComp->GetMaxHP();

                float NewHP = FMath::Clamp(CurrentHP + HealAmount, 0.f, MaxHP);

                StatusComp->SetCurrentHP(NewHP);

                UE_LOG(LogTemp, Log, TEXT("Heal Applied: %.2f -> %.2f (Max: %.2f)"), CurrentHP, NewHP, MaxHP);
            }

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HandleEnemyKilled: Failed to find Data in DataTable for HealOnKill"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HandleEnemyKilled: HealOnKill NOT FOUND in Map."));
    }

    // 5. Specific Logic Check: AmmoCycle (Enum 5)
    if (OwnedAugments.Contains(EAugmentType::AmmoCycle))
    {
        const FAugmentTableData* Data = GetAugmentData(EAugmentType::AmmoCycle);
        if (Data)
        {
            int32 Level = OwnedAugments[EAugmentType::AmmoCycle];
            float AmmoAmount = Data->BaseValue + (Level - 1) * Data->UpgradeValue;
            int32 RestoreAmount = FMath::FloorToInt(AmmoAmount);

            // 부모 클래스인 ACharacterBase로 캐스팅합니다! (Player든 NPC든 다 커버 가능)
            ACharacterBase* BaseChar = Cast<ACharacterBase>(OwnerChar);
            if (BaseChar)
            {
                // 부모한테 물려받은 GetCurrentWeapon() 함수를 당당하게 호출!
                /*AWeapon* EquippedWeapon = BaseChar->GetCurrentWeapon();

                if (EquippedWeapon)
                {
                    int32 CurrentAmmo = EquippedWeapon->GetCurrentBullets();
                    int32 MaxAmmo = 30; // 무기 클래스에 MaxBullets가 있다면 나중에 연동하세요!

                    int32 NewAmmo = FMath::Clamp(CurrentAmmo + RestoreAmount, 0, MaxAmmo);

                    // 무기 총알 세팅 (오타 매칭 함수)
                    EquippedWeapon->SetCurrentBullsets(NewAmmo);

                    // UI 갱신은 '플레이어'일 때만 필요하므로, 여기서 플레이어로 한 번 더 체크해줍니다.
                    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(BaseChar);
                    if (PlayerChar)
                    {
                        PlayerChar->OnAmmoChanged(NewAmmo, MaxAmmo);
                    }

                    UE_LOG(LogTemp, Log, TEXT(">>> AmmoCycle Success: Ammo Recharged! %d -> %d <<<"), CurrentAmmo, NewAmmo);
                }
                */
            }
        }
    }

}

const FAugmentTableData* UAugmentComponent::GetAugmentData(EAugmentType Type)
{
    if (!AugmentDataTable) return nullptr;


    TArray<FAugmentTableData*> AllRows;
    AugmentDataTable->GetAllRows<FAugmentTableData>(TEXT("GetAugmentData Context"), AllRows);
    for (auto Row : AllRows)
    {
        if (Row && Row->Type == Type)
        {
            return Row;
        }
    }

    return nullptr;
}

void UAugmentComponent::CheckLowHPSpeedBuff(float CurrentHP)
{
    if (!OwnedAugments.Contains(EAugmentType::CrisisInstinct)) return;

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    UStatusComponent* StatusComp = GetOwner()->FindComponentByClass<UStatusComponent>();

    if (OwnerChar && StatusComp)
    {
        UCharacterMovementComponent* MoveComp = OwnerChar->GetCharacterMovement();
        if (!MoveComp) return;

        float MaxHP = StatusComp->GetMaxHP();
        float HPRatio = (MaxHP > 0.f) ? (CurrentHP / MaxHP) : 1.f;

        const FAugmentTableData* Data = GetAugmentData(EAugmentType::CrisisInstinct);
        if (!Data) return;

        int32 Level = OwnedAugments[EAugmentType::CrisisInstinct];
        float BonusSpeed = Data->BaseValue + (Level - 1) * Data->UpgradeValue;

        // 아까 캐릭터에 작성되어 있다던 baseSpeed 변수를 가져옵니다.
        // 만약 baseSpeed가 PlayerCharacter에만 있다면 캐스팅을, CharacterBase에 있다면 거기로 캐스팅해야 합니다.
        // 여기서는 일단 변수가 선언된 캐릭터로 캐스팅했다고 가정합니다.
        APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OwnerChar);
        if (!PlayerChar) return;

        // 3. 체력이 30% 이하이고 버프가 안 켜졌다면 -> 속도 즉시 반영!
        if (HPRatio <= 0.3f && !bIsSpeedBuffActive)
        {
            bIsSpeedBuffActive = true;

            float NewSpeed = PlayerChar->baseSpeed + BonusSpeed;

            // 변수 세팅과 실제 무브먼트 컴포넌트 속도 제어를 컴포넌트 안에서 다 처리합니다!
            //PlayerChar->CurrentSpeed = NewSpeed;
            MoveComp->MaxWalkSpeed = NewSpeed;

            UE_LOG(LogTemp, Warning, TEXT(">>> LOW HP! (Component) Speed Buff Active: %.2f <<<"), NewSpeed);
        }
        // 4. 체력이 30%를 초과했고 버프가 켜져 있다면 -> 원상복구!
        else if (HPRatio > 0.3f && bIsSpeedBuffActive)
        {
            bIsSpeedBuffActive = false;

            //PlayerChar->CurrentSpeed = PlayerChar->baseSpeed;
            MoveComp->MaxWalkSpeed = PlayerChar->baseSpeed;

            UE_LOG(LogTemp, Log, TEXT(">>> HP Recovered. (Component) Speed Buff Deactivated. <<<"));
        }
    }
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