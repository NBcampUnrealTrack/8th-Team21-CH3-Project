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
        ActiveWidget->OnAugmentSelected.AddDynamic(this, &UAugmentComponent::OnAugmentCardSelected);
    }
}

void UAugmentComponent::OnAugmentCardSelected(FAugmentResult SelectedCardData)
{
    // 1. 선택된 카드 능력치 적용 (기존 로직 유지)
    ApplyAugment(SelectedCardData.Type);

    // 2. 켜져 있던 강화 위젯 제거 및 초기화
    if (IsValid(ActiveWidget))
    {
        ActiveWidget->RemoveFromParent();
        ActiveWidget = nullptr;
    }

    // 3. ◀ 핵심 추가: 월드에서 게임모드를 찾아 선택 완료 알림을 보냅니다.
    if (UWorld* World = GetWorld())
    {
        AShooterInGameMode* GM = Cast<AShooterInGameMode>(World->GetAuthGameMode());
        if (GM)
        {
            // 게임모드가 일시정지를 풀고, 마우스를 숨기고, 
            // 5킬 시점에 밀려있던 Wave 클리어 사이클을 마저 실행합니다.
            GM->NotifyAugmentSelectionComplete();
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


    if (OwnedAugments.Contains(EAugmentType::AmmoCycle))
    {
        const FAugmentTableData* Data = GetAugmentData(EAugmentType::AmmoCycle);
        if (Data)
        {
            int32 Level = OwnedAugments[EAugmentType::AmmoCycle];
            float AmmoAmount = Data->BaseValue + (Level - 1) * Data->UpgradeValue;
            int32 RestoreAmount = FMath::FloorToInt(AmmoAmount);

            ACharacterBase* BaseChar = Cast<ACharacterBase>(OwnerChar);
            if (BaseChar)
            {
                AWeapon* EquippedWeapon = BaseChar->CurrentWeapon.Get();

                if (EquippedWeapon)
                {
                    int32 CurrentAmmo = EquippedWeapon->GetCurrentBullets();
                    int32 MaxAmmo = EquippedWeapon->GetMaxBullets();

                    int32 NewAmmo = FMath::Clamp(CurrentAmmo + RestoreAmount, 0, MaxAmmo);

                    EquippedWeapon->SetCurrentBullsets(NewAmmo);
                    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(BaseChar);
                    if (PlayerChar)
                    {
                        PlayerChar->OnAmmoChanged(NewAmmo, MaxAmmo);
                    }

                    UE_LOG(LogTemp, Log, TEXT(">>> AmmoCycle Success: Ammo Recharged! %d -> %d <<<"), CurrentAmmo, NewAmmo);
                }
                
            }
        }
    }
    
    if (OwnedAugments.Contains(EAugmentType::ChainKill))
    {
        KillCount++;

        if (KillCount >= 2)
        {
            bIsChainKillActive = true;
        }
        
        


        GetWorld()->GetTimerManager().SetTimer(
            ChainKillTimerHandle,
            this,
            &UAugmentComponent::ResetchainKill,
            5.0f,
            false
        );
    }
}


void UAugmentComponent::AugmentSelection()
{
    if (!AugmentWidgetClass) return;

    UWorld* World = GetWorld();
    APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(World, 0));
    if (!PC) return;

    TArray<FAugmentResult> FinalOptions = RollRandomAugmentOptions();
    if (FinalOptions.Num() == 0) return;

    ActiveAugmentWidget = CreateWidget<UAugmentCardSelectWidget>(PC, AugmentWidgetClass);
    if (ActiveAugmentWidget)
    {
        ActiveAugmentWidget->OnDataReceived(FinalOptions);
        ActiveAugmentWidget->AddToViewport(200);

        BindAugmentWidget(ActiveAugmentWidget);
    }
}

void UAugmentComponent::ResetchainKill()
{
    bIsChainKillActive = false;
    KillCount = 0;
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

TArray<FAugmentResult> UAugmentComponent::RollRandomAugmentOptions()
{
    TArray<FAugmentResult> GeneratedOptions;
    if (!AugmentDataTable) return GeneratedOptions;

    TArray<FName> RowNames = AugmentDataTable->GetRowNames();
    if (RowNames.Num() < 3) return GeneratedOptions;

    for (int32 i = RowNames.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        RowNames.Swap(i, j);
    }

    for (int32 i = 0; i < 3; i++)
    {
        FAugmentTableData* Row = AugmentDataTable->FindRow<FAugmentTableData>(RowNames[i], TEXT(""));
        if (!Row) continue;

        FAugmentResult Option;
        Option.Type = Row->Type;

        Option.icon = Row->icon;

        int32 CurrentLevel = GetAugmentLevel(Row->Type);
        Option.CurrentLevel = CurrentLevel + 1;
        Option.DisplayTitle = FString::Printf(TEXT("%s (Lv.%d)"), *Row->AugmentName, Option.CurrentLevel);

        float DisplayValue = Row->BaseValue + (CurrentLevel * Row->UpgradeValue);

        FFormatNamedArguments Args;
        Args.Add(TEXT("Value"), FText::AsNumber(FMath::FloorToInt(DisplayValue)));
        Args.Add(TEXT("Unit"), FText::FromString(Row->UnitText));
        Option.Description = FText::Format(FText::FromString(Row->DescriptionFormat), Args).ToString();

        GeneratedOptions.Add(Option);
    }

    return GeneratedOptions;
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


        APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OwnerChar);
        if (!PlayerChar) return;


        if (HPRatio <= 0.3f && !bIsSpeedBuffActive)
        {
            bIsSpeedBuffActive = true;

            float NewSpeed = PlayerChar->baseSpeed * (1.f + BonusSpeed / 100.f);
            float NewTargetSpeed = PlayerChar->baseTargetSpeed * (1.f + BonusSpeed / 100.f);


            PlayerChar->CurrentSpeed = NewSpeed;
            PlayerChar->TargetSpeed = NewTargetSpeed;
            if (PlayerChar->CurrentSpeed == PlayerChar->baseTargetSpeed)
            {
                PlayerChar->CurrentSpeed = NewTargetSpeed;
                MoveComp->MaxWalkSpeed = NewTargetSpeed;
            }
            else
            {
                PlayerChar->CurrentSpeed = NewSpeed;
                MoveComp->MaxWalkSpeed = NewSpeed;
            }

            UE_LOG(LogTemp, Warning, TEXT(">>> LOW HP! (Component) Speed Buff Active: %.2f <<<"), NewSpeed);
        }

        else if (HPRatio > 0.3f && bIsSpeedBuffActive)
        {
            bIsSpeedBuffActive = false;

            PlayerChar->CurrentSpeed = PlayerChar->baseSpeed;
            PlayerChar->TargetSpeed = PlayerChar->baseTargetSpeed;
            if (MoveComp->MaxWalkSpeed > PlayerChar->baseTargetSpeed)
            {
                PlayerChar->CurrentSpeed = PlayerChar->baseTargetSpeed;
                MoveComp->MaxWalkSpeed = PlayerChar->baseTargetSpeed;
            }
            else
            {
                PlayerChar->CurrentSpeed = PlayerChar->baseSpeed;
                MoveComp->MaxWalkSpeed = PlayerChar->baseSpeed;
            }

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

    //ChainKill
    if (OwnedAugments.Contains(EAugmentType::ChainKill))
    {
        if (bIsChainKillActive)
        {
            const FAugmentTableData* Data = GetAugmentData(EAugmentType::ChainKill);
            if (Data)
            {
                int32 Level = OwnedAugments[EAugmentType::ChainKill];
                float BonusValue = (Data->BaseValue + (Level - 1) * Data->UpgradeValue) / 100.0f;
                FinalModifier += BonusValue;

                UE_LOG(LogTemp, Log, TEXT("ChainKill Activate!"));
            }
        }
    }

    //ReloadReward
    if (OwnedAugments.Contains(EAugmentType::ReloadReward))
    {
        if (bIsReloadRewardActive) 
        {
            const FAugmentTableData* Data = GetAugmentData(EAugmentType::ReloadReward);
            if (Data)
            {
                int32 Level = OwnedAugments[EAugmentType::ReloadReward];
                float BonusValue = (Data->BaseValue + (Level - 1) * Data->UpgradeValue) / 100.0f;
                FinalModifier += BonusValue;

                bIsReloadRewardActive = false;

            }
        }
        
    }

    return InBaseDamage * FinalModifier;
}