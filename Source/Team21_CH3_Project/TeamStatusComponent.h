// TeamStatusComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamStatusComponent.generated.h"

// MaxHP가 변경되었을 때 UI에 알려주는 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedSignature, float, NewMaxHP);

// CurrentHP가 변경되었을 때 UI에 알려주는 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedSignature, float, NewCurrentHP);

// HP가 0이 되었을 때 사망 처리를 알려주는 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM21_CH3_PROJECT_API UTeamStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTeamStatusComponent();

protected:
	virtual void BeginPlay() override;

public:
	// MaxHP 변경 알림
	UPROPERTY(BlueprintAssignable, Category = "Status|HP")
	FOnMaxHPChangedSignature OnMaxHPChanged;

	// CurrentHP 변경 알림
	UPROPERTY(BlueprintAssignable, Category = "Status|HP")
	FOnCurrentHPChangedSignature OnCurrentHPChanged;

	// 사망 알림
	UPROPERTY(BlueprintAssignable, Category = "Status|Death")
	FOnDeathSignature OnDeath;

public:
	// 최대 HP 반환
	UFUNCTION(BlueprintPure, Category = "Status|HP")
	float GetMaxHP() const;

	// 현재 HP 반환
	UFUNCTION(BlueprintPure, Category = "Status|HP")
	float GetCurrentHP() const;

	// 최대 HP 설정
	UFUNCTION(BlueprintCallable, Category = "Status|HP")
	void SetMaxHP(float NewMaxHP);

	// 현재 HP 설정
	UFUNCTION(BlueprintCallable, Category = "Status|HP")
	void SetCurrentHP(float NewCurrentHP);

	// 데미지 적용
	UFUNCTION(BlueprintCallable, Category = "Status|HP")
	void ApplyDamage(float DamageAmount);

	// 사망 여부 반환
	UFUNCTION(BlueprintPure, Category = "Status|Death")
	bool IsDead() const;

private:
	// 최대 HP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status|HP", meta = (AllowPrivateAccess = "true"))
	float MaxHP;

	// 현재 HP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|HP", meta = (AllowPrivateAccess = "true"))
	float CurrentHP;

	// 사망 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status|Death", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;
};