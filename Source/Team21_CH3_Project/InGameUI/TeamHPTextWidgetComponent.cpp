// TeamHPTextWidgetComponent.cpp

#include "TeamHPTextWidgetComponent.h"

#include "UW_HPText.h"
#include "Component/StatusComponent.h"

UTeamHPTextWidgetComponent::UTeamHPTextWidgetComponent()
{
	// AI/캐릭터 머리 위에 표시할 UI이므로 Screen Space 사용
	SetWidgetSpace(EWidgetSpace::Screen);

	// WBP_HPText 기준 크기
	SetDrawSize(FVector2D(200.f, 40.f));

	// UI 컴포넌트이므로 충돌은 사용하지 않는다.
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UTeamHPTextWidgetComponent::InitWidget()
{
	Super::InitWidget();

	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	UUW_HPText* HPTextWidgetInstance = Cast<UUW_HPText>(GetWidget());
	if (!IsValid(HPTextWidgetInstance))
	{
		return;
	}

	HPTextWidgetInstance->SetOwningActor(OwnerActor);

	UStatusComponent* StatusComponent = OwnerActor->FindComponentByClass<UStatusComponent>();
	if (!IsValid(StatusComponent))
	{
		return;
	}

	HPTextWidgetInstance->InitializeHPTextWidget(StatusComponent);
}