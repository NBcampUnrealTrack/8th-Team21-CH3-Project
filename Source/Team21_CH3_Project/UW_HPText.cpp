// UW_HPText.cpp

#include "UW_HPText.h"

#include "Component/StatusComponent.h"
#include "Components/TextBlock.h"

UUW_HPText::UUW_HPText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OwningActor = nullptr;
	BoundStatusComponent = nullptr;
}

void UUW_HPText::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_HPText::NativeDestruct()
{
	// 위젯이 제거될 때 기존 StatusComponent에 연결된 델리게이트를 해제한다.
	if (BoundStatusComponent)
	{
		BoundStatusComponent->OnMaxHPChanged.RemoveAll(this);
		BoundStatusComponent->OnCurrentHPChanged.RemoveAll(this);
	}

	BoundStatusComponent = nullptr;

	Super::NativeDestruct();
}

void UUW_HPText::InitializeHPTextWidget(UStatusComponent* InStatusComponent)
{
	if (!InStatusComponent)
	{
		return;
	}

	BoundStatusComponent = InStatusComponent;

	// 최대체력이 변하면 MaxHP에 관련된 함수들에게 알림
	// StatusComponent의 OnMaxHPChanged 델리게이트에 HP Text 갱신 함수를 연결한다.
	BoundStatusComponent->OnMaxHPChanged.AddUObject(this, &UUW_HPText::OnMaxHPChange);

	// 현재체력의 변화를 알림
	// StatusComponent의 OnCurrentHPChanged 델리게이트에 HP Text 갱신 함수를 연결한다.
	BoundStatusComponent->OnCurrentHPChanged.AddUObject(this, &UUW_HPText::OnCurrentHPChange);

	// 위젯이 처음 생성될 때 현재 HP 값을 즉시 반영한다.
	OnMaxHPChange(BoundStatusComponent->GetMaxHP());
	OnCurrentHPChange(BoundStatusComponent->GetCurrentHP());
}

void UUW_HPText::OnMaxHPChange(float InMaxHP)
{
	if (!MaxHPText)
	{
		return;
	}

	const int32 DisplayMaxHP = FMath::RoundToInt(InMaxHP);
	MaxHPText->SetText(FText::AsNumber(DisplayMaxHP));
}

void UUW_HPText::OnCurrentHPChange(float InCurrentHP)
{
	if (!CurrentHPText)
	{
		return;
	}

	const int32 DisplayCurrentHP = FMath::RoundToInt(InCurrentHP);
	CurrentHPText->SetText(FText::AsNumber(DisplayCurrentHP));
}