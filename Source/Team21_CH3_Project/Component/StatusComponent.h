//StatusComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM21_CH3_PROJECT_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatusComponent();

	float ApplyDamage(float InDamage);
		//데미지 적용 

	float GetMaxHP() const { return MaxHP; }
	void SetMaxHP(float InMaxHP);
		//최대체력
	float GetCurrentHP() const { return CurrentHP; }
	void SetCurrentHP(float InCurrentHP);
		//현재 체력
	bool IsDead() const { return bIsDead; }
		//죽음 여부
public:
	UPROPERTY()
	float MaxHP = 100.f;

	UPROPERTY(Transient)
	float CurrentHP = 100.f;

	UPROPERTY(Transient)
	uint8 bIsDead : 1;

	FOnMaxHPChangedDelegate OnMaxHPChanged;

	FOnCurrentHPChangedDelegate OnCurrentHPChanged;

	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;
	//델리게이트 변수
};
