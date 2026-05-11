// UW_HPText.cpp

#include "UW_HPText.h"

#include "TeamStatusComponent.h"
#include "Components/TextBlock.h"

UUW_HPText::UUW_HPText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OwningActor = nullptr;
}

void UUW_HPText::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_HPText::InitializeHPTextWidget(UTeamStatusComponent* InStatusComponent)
{
	if (!InStatusComponent)
	{
		return;
	}

	// 위젯이 처음 생성될 때 TeamStatusComponent의 현재 값을 즉시 반영한다.
	OnMaxHPChange(InStatusComponent->GetMaxHP());
	OnCurrentHPChange(InStatusComponent->GetCurrentHP());

	// 이후 HP 변경을 자동으로 반영하고 싶을 때 Delegate를 연결한다.
	// TeamStatusComponent에 Delegate가 준비되면 아래 주석을 해제해서 사용하면 된다.
	//
	// InStatusComponent->OnMaxHPChanged.AddDynamic(this, &UUW_HPText::OnMaxHPChange);
	// InStatusComponent->OnCurrentHPChanged.AddDynamic(this, &UUW_HPText::OnCurrentHPChange);
}

void UUW_HPText::OnMaxHPChange(float InMaxHP)
{
	if (!MaxHPText)
	{
		return;
	}

	// float HP 값을 정수로 반올림해서 UI에 표시한다.
	const int32 DisplayMaxHP = FMath::RoundToInt(InMaxHP);
	MaxHPText->SetText(FText::AsNumber(DisplayMaxHP));
}

void UUW_HPText::OnCurrentHPChange(float InCurrentHP)
{
	if (!CurrentHPText)
	{
		return;
	}

	// float HP 값을 정수로 반올림해서 UI에 표시한다.
	const int32 DisplayCurrentHP = FMath::RoundToInt(InCurrentHP);
	CurrentHPText->SetText(FText::AsNumber(DisplayCurrentHP));
}