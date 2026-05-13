// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AugmentationDataTable.generated.h"

UENUM(BlueprintType)
enum class EAugmentType : uint8
{
    HealOnKill,
    ChainKill,
    ClosePressure,
    ReloadReward,
    CrisisInstinct,
    AmmoCycle,
    BossHunter
};

USTRUCT(BlueprintType)
struct FAugmentTableData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAugmentType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AugmentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
    FString DescriptionFormat; // 예: "Enemy 처치 시 HP %s 회복"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseValue;      // 처음 획득 시 수치

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float UpgradeValue;   // 중복 획득 시 추가되는 수치

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString UnitText;     // "회복", "%", "발" 등
};

USTRUCT(BlueprintType)
struct FAugmentResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    EAugmentType Type;

    UPROPERTY(BlueprintReadWrite)
    FString DisplayTitle;

    UPROPERTY(BlueprintReadWrite)
    FString Description;

    UPROPERTY(BlueprintReadWrite)
    int32 CurrentLevel; // 현재 몇 단계인지 UI에 표시용
};
