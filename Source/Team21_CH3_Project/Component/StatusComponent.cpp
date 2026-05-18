//StatusComponent.cpp

#include "Component/StatusComponent.h"

UStatusComponent::UStatusComponent(): 
	bIsDead(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	PrimaryComponentTick.bCanEverTick = false;

}

float UStatusComponent::ApplyDamage(float InDamage)
{
	const float PreviousHP = CurrentHP; //데미지 입기 전 체력
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, PreviousHP);
	//실제 데미지 (받은데미지, 0 ~ PreviousHP 사이의 값으로 강제)

	SetCurrentHP(PreviousHP - ActualDamage);
	//데미지 로직 후 현재 체력 = 이전체력 - 입은 데미지

	return ActualDamage;
}

void UStatusComponent::SetMaxHP(float InMaxHP)
{
	MaxHP = InMaxHP;

	if (MaxHP < KINDA_SMALL_NUMBER)
	{
		MaxHP = 0.1f;
	}

	OnMaxHPChanged.Broadcast(MaxHP);
	//최대체력이 변하면 MaxHP에 관련된 함수들에게 알림
}

void UStatusComponent::SetCurrentHP(float InCurrentHP)
{
	CurrentHP = InCurrentHP;
	if (CurrentHP <= KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.f;
		OnOutOfCurrentHP.Broadcast();
		//체력이 0이 되었음을 알림
		bIsDead = true;
	}
	OnCurrentHPChanged.Broadcast(CurrentHP);
	//현재체력의 변화를 알림
}
