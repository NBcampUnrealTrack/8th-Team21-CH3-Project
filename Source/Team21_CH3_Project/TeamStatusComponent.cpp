// TeamStatusComponent.cpp

#include "TeamStatusComponent.h"

UTeamStatusComponent::UTeamStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHP = 100.f;
	CurrentHP = MaxHP;
	bIsDead = false;
}

void UTeamStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	bIsDead = false;

	// BeginPlay 시점에 초기 HP 값을 UI 쪽에 알림
	OnMaxHPChanged.Broadcast(MaxHP);
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

float UTeamStatusComponent::GetMaxHP() const
{
	return MaxHP;
}

float UTeamStatusComponent::GetCurrentHP() const
{
	return CurrentHP;
}

void UTeamStatusComponent::SetMaxHP(float NewMaxHP)
{
	// MaxHP는 최소 1 이상
	MaxHP = FMath::Max(1.f, NewMaxHP);

	// 현재 HP가 MaxHP보다 커지지 않게 보정
	CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	// 변경된 값을 UI에 알림
	OnMaxHPChanged.Broadcast(MaxHP);
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UTeamStatusComponent::SetCurrentHP(float NewCurrentHP)
{
	if (bIsDead)
	{
		return;
	}

	CurrentHP = FMath::Clamp(NewCurrentHP, 0.f, MaxHP);

	// 현재 HP 변경을 UI에 알림
	OnCurrentHPChanged.Broadcast(CurrentHP);

	// HP가 0이면 사망 처리
	if (CurrentHP <= 0.f)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

void UTeamStatusComponent::ApplyDamage(float DamageAmount)
{
	if (bIsDead)
	{
		return;
	}

	if (DamageAmount <= 0.f)
	{
		return;
	}

	SetCurrentHP(CurrentHP - DamageAmount);
}

bool UTeamStatusComponent::IsDead() const
{
	return bIsDead;
}