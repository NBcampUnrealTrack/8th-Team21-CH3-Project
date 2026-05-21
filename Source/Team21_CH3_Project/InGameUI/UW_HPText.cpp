// UW_HPText.cpp

#include "UW_HPText.h"

#include "Component/StatusComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

UUW_HPText::UUW_HPText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OwningActor(nullptr)
	, BoundStatusComponent(nullptr)
	, bIsStatusDelegateBound(false)
{
}

void UUW_HPText::NativeConstruct()
{
	Super::NativeConstruct();

	BindStatusDelegates();
	UpdateHPDisplay();
}

void UUW_HPText::NativeDestruct()
{
	// WidgetComponent의 UserWidget은 표시 상태나 재구성 과정에서
	// NativeConstruct / NativeDestruct가 여러 번 호출될 수 있다.
	// 여기서 델리게이트를 해제하면 HP Bar 갱신이 끊길 수 있으므로 해제하지 않는다.

	Super::NativeDestruct();
}

void UUW_HPText::BeginDestroy()
{
	UnbindStatusDelegates();

	Super::BeginDestroy();
}

void UUW_HPText::InitializeHPTextWidget(UStatusComponent* InStatusComponent)
{
	if (!IsValid(InStatusComponent))
	{
		return;
	}

	if (BoundStatusComponent && BoundStatusComponent != InStatusComponent)
	{
		UnbindStatusDelegates();
	}

	BoundStatusComponent = InStatusComponent;

	BindStatusDelegates();
	UpdateHPDisplay();
}

void UUW_HPText::BindStatusDelegates()
{
	if (!IsValid(BoundStatusComponent))
	{
		return;
	}

	if (bIsStatusDelegateBound)
	{
		return;
	}

	BoundStatusComponent->OnMaxHPChanged.AddUObject(this, &UUW_HPText::OnMaxHPChange);
	BoundStatusComponent->OnCurrentHPChanged.AddUObject(this, &UUW_HPText::OnCurrentHPChange);

	bIsStatusDelegateBound = true;
}

void UUW_HPText::UnbindStatusDelegates()
{
	if (!IsValid(BoundStatusComponent))
	{
		bIsStatusDelegateBound = false;
		return;
	}

	if (!bIsStatusDelegateBound)
	{
		return;
	}

	BoundStatusComponent->OnMaxHPChanged.RemoveAll(this);
	BoundStatusComponent->OnCurrentHPChanged.RemoveAll(this);

	bIsStatusDelegateBound = false;
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
	if (!IsValid(BoundStatusComponent))
	{
		return;
	}

	const float MaxHP = BoundStatusComponent->GetMaxHP();
	const float CurrentHP = BoundStatusComponent->GetCurrentHP();

	if (MaxHP <= 0.0f)
	{
		if (HPBar)
		{
			HPBar->SetPercent(0.0f);
		}

		if (CurrentHPText)
		{
			CurrentHPText->SetText(FText::AsNumber(0));
		}

		// 최대 체력 텍스트는 사용하지 않는다.
		if (MaxHPText)
		{
			MaxHPText->SetText(FText::GetEmpty());
			MaxHPText->SetVisibility(ESlateVisibility::Collapsed);
		}

		return;
	}

	const float SafeCurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);
	const float HPPercent = SafeCurrentHP / MaxHP;

	if (HPBar)
	{
		HPBar->SetPercent(HPPercent);
	}

	// 현재 HP만 표시한다.
	if (CurrentHPText)
	{
		CurrentHPText->SetVisibility(ESlateVisibility::HitTestInvisible);
		CurrentHPText->SetText(FText::AsNumber(FMath::RoundToInt(SafeCurrentHP)));
	}

	// 기존 MaxHPText는 숨긴다.
	if (MaxHPText)
	{
		MaxHPText->SetText(FText::GetEmpty());
		MaxHPText->SetVisibility(ESlateVisibility::Collapsed);
	}
}