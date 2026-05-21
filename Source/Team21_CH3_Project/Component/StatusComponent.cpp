// StatusComponent.cpp

#include "Component/StatusComponent.h"

UStatusComponent::UStatusComponent()
	: bIsDead(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UStatusComponent::ApplyDamage(float InDamage)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	const float PreviousHP = CurrentHP;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0.0f, PreviousHP);

	SetCurrentHP(PreviousHP - ActualDamage);

	return ActualDamage;
}

void UStatusComponent::SetMaxHP(float InMaxHP)
{
	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	if (CurrentHP > MaxHP)
	{
		CurrentHP = MaxHP;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
	OnCurrentHPChanged.Broadcast(CurrentHP);
}

void UStatusComponent::SetCurrentHP(float InCurrentHP)
{
	CurrentHP = FMath::Clamp<float>(InCurrentHP, 0.0f, MaxHP);

	OnCurrentHPChanged.Broadcast(CurrentHP);

	if (CurrentHP <= KINDA_SMALL_NUMBER && !bIsDead)
	{
		CurrentHP = 0.0f;
		bIsDead = true;

		OnOutOfCurrentHP.Broadcast();
	}
}