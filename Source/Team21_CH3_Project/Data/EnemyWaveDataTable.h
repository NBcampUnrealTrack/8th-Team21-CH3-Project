#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemyWaveDataTable.generated.h"

UENUM(BlueprintType)
enum class EWaveRewardType : uint8
{
    Shop    UMETA(DisplayName = "Shop"),
    Result  UMETA(DisplayName = "Result")
};

USTRUCT(BlueprintType)
struct FEnemyWaveDataTable : public FTableRowBase
{
    GENERATED_BODY()

public:
    FEnemyWaveDataTable()
        : WaveIndex(0), NormalCount(0), RusherCount(0), ShooterCount(0)
        , BossCount(0), SpawnInterval(1.0f), StatMultiplier(1.0f), GoldPerKill(10)
        , RewardAfterClear(EWaveRewardType::Shop)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 WaveIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    int32 NormalCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    int32 RusherCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    int32 ShooterCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    int32 BossCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float SpawnInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float StatMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    int32 GoldPerKill;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
    EWaveRewardType RewardAfterClear;
};