// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterStat.generated.h"

/**
 * 
 */
class TEAM21_CH3_PROJECT_API MonsterStat
{
	MonsterStat();
	~MonsterStat();
public:
};
UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	None UMETA(DisplayName = "Enemy_None"),
	Normal UMETA(DisplayName = "Enemy_Normal"),
	Rusher UMETA(DisplayName = "Enemy_Rusher"),
	Shooter UMETA(DisplayName = "Enemy_Shooter")
};


USTRUCT(Atomic, BlueprintType)
struct FMonsterStat
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMonsterType MonsterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

};