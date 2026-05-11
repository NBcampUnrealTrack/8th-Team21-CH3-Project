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

	// WidgetComponent가 생성한 실제 위젯을 UW_HPText로 캐스팅
	UUW_HPText* HPTextWidgetInstance = Cast<UUW_HPText>(GetWidget());
	if (!IsValid(HPTextWidgetInstance))
	{
		return;
	}

	// 이 HP 위젯을 소유한 액터 저장
	HPTextWidgetInstance->SetOwningActor(OwnerActor);

	// OwnerActor가 가지고 있는 기존 StatusComponent를 찾는다.
	UStatusComponent* StatusComponent = OwnerActor->FindComponentByClass<UStatusComponent>();
	if (!IsValid(StatusComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("TeamHPTextWidgetComponent: StatusComponent not found on %s"), *OwnerActor->GetName());
		return;
	}

	// StatusComponent의 CurrentHP / MaxHP 값을 HP UI에 연결한다.
	HPTextWidgetInstance->InitializeHPTextWidget(StatusComponent);
}