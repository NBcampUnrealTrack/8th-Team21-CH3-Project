// UW_HPText.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HPText.generated.h"

class UStatusComponent;
class UTextBlock;

// AI 머리 위에 표시할 HP Text 위젯
UCLASS()
class TEAM21_CH3_PROJECT_API UUW_HPText : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_HPText(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// HP 정보를 가진 StatusComponent를 받아서 HP Text를 초기화한다.
	void InitializeHPTextWidget(UStatusComponent* InStatusComponent);

	// 이 HP 위젯을 소유한 액터 반환
	AActor* GetOwningActor() const { return OwningActor; }

	// 이 HP 위젯을 소유한 액터 저장
	void SetOwningActor(AActor* InOwningActor) { OwningActor = InOwningActor; }

	// MaxHP가 변경되었을 때 호출될 함수
	void OnMaxHPChange(float InMaxHP);

	// CurrentHP가 변경되었을 때 호출될 함수
	void OnCurrentHPChange(float InCurrentHP);

protected:
	// 현재 HP를 표시할 TextBlock
	// WBP_HPText 안에 같은 이름의 TextBlock이 있어야 BindWidget으로 연결된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;

	// 최대 HP를 표시할 TextBlock
	// WBP_HPText 안에 같은 이름의 TextBlock이 있어야 BindWidget으로 연결된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;

	// 이 위젯을 소유한 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;

	// 연결된 HP/상태 컴포넌트
	UPROPERTY()
	TObjectPtr<UStatusComponent> BoundStatusComponent;
};