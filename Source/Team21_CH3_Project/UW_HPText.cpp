// UW_HPText.cpp

#include "UW_HPText.h"

#include "Component/StatusComponent.h"
#include "Components/ProgressBar.h"
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
	// StatusComponent의 OnMaxHPChanged 델리게이트에 HP UI 갱신 함수를 연결한다.
	BoundStatusComponent->OnMaxHPChanged.AddUObject(this, &UUW_HPText::OnMaxHPChange);

	// 현재체력의 변화를 알림
	// StatusComponent의 OnCurrentHPChanged 델리게이트에 HP UI 갱신 함수를 연결한다.
	BoundStatusComponent->OnCurrentHPChanged.AddUObject(this, &UUW_HPText::OnCurrentHPChange);

	// 위젯이 처음 생성될 때 현재 HP 값을 즉시 반영한다.
	UpdateHPDisplay();
}

void UUW_HPText::OnMaxHPChange(float InMaxHP)
{
	UpdateHPDisplay();
}

void UUW_HPText::OnCurrentHPChange(float InCurrentHP)
{
	UpdateHPDisplay();
}

void UUW_HPText::UpdateHPDisplay()
{
	if (!BoundStatusComponent)
	{
		return;
	}

	const float MaxHP = BoundStatusComponent->GetMaxHP();
	const float CurrentHP = BoundStatusComponent->GetCurrentHP();

	// MaxHP가 잘못된 값이면 안전하게 0으로 표시한다.
	if (MaxHP <= 0.f)
	{
		if (HPBar)
		{
			HPBar->SetPercent(0.f);
		}

		if (CurrentHPText)
		{
			CurrentHPText->SetText(FText::AsNumber(0));
		}

		if (MaxHPText)
		{
			MaxHPText->SetText(FText::AsNumber(0));
		}

		return;
	}

	// CurrentHP가 0 ~ MaxHP 범위를 벗어나지 않도록 보정
	const float SafeCurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

	// ProgressBar는 0.0 ~ 1.0 값을 사용하므로 비율로 변환
	const float HPPercent = SafeCurrentHP / MaxHP;

	// HP Bar 갱신
	if (HPBar)
	{
		HPBar->SetPercent(HPPercent);
	}

	// 현재 HP Text 갱신
	if (CurrentHPText)
	{
		CurrentHPText->SetText(FText::AsNumber(FMath::RoundToInt(SafeCurrentHP)));
	}

	// 최대 HP Text 갱신
	if (MaxHPText)
	{
		MaxHPText->SetText(FText::AsNumber(FMath::RoundToInt(MaxHP)));
	}
}