// UW_HPText.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HPText.generated.h"

class UStatusComponent;
class UTextBlock;
class UProgressBar;

// AI 머리 위에 표시할 HP UI 위젯
// HPBar + CurrentHPText + MaxHPText를 함께 갱신한다.
UCLASS()
class TEAM21_CH3_PROJECT_API UUW_HPText : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_HPText(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// HP 정보를 가진 StatusComponent를 받아서 HP UI를 초기화한다.
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
	// HP 비율을 표시할 ProgressBar
	// WBP_HPText 안의 ProgressBar 이름이 HPBar여야 연결된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	// 현재 HP를 표시할 TextBlock
	// WBP_HPText 안의 TextBlock 이름이 CurrentHPText여야 연결된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;

	// 최대 HP를 표시할 TextBlock
	// WBP_HPText 안의 TextBlock 이름이 MaxHPText여야 연결된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;

	// 이 위젯을 소유한 액터
	// 예: AI 캐릭터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;

	// 연결된 기존 HP/상태 컴포넌트
	UPROPERTY()
	TObjectPtr<UStatusComponent> BoundStatusComponent;

private:
	// StatusComponent의 현재 값을 기준으로 HPBar와 HP Text를 한 번에 갱신한다.
	void UpdateHPDisplay();
};