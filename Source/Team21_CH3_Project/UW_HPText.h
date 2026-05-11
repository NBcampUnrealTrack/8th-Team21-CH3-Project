// UW_HPText.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HPText.generated.h"

class UTeamStatusComponent;
class UTextBlock;

// AI 머리 위에 표시할 HP Text 위젯
// WBP_HPText의 부모 클래스로 사용된다.
UCLASS()
class TEAM21_CH3_PROJECT_API UUW_HPText : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_HPText(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	// HP 정보를 가진 TeamStatusComponent를 받아서
	// 현재 HP / 최대 HP Text를 초기화한다.
	void InitializeHPTextWidget(UTeamStatusComponent* InStatusComponent);

	// 이 HP 위젯을 소유한 액터 반환
	AActor* GetOwningActor() const { return OwningActor; }

	// 이 HP 위젯을 소유한 액터 저장
	// WidgetComponent의 InitWidget()에서 호출된다.
	void SetOwningActor(AActor* InOwningActor) { OwningActor = InOwningActor; }

	// MaxHP가 변경되었을 때 호출될 함수
	// MaxHPText에 최대 HP 값을 표시한다.
	UFUNCTION()
	void OnMaxHPChange(float InMaxHP);

	// CurrentHP가 변경되었을 때 호출될 함수
	// CurrentHPText에 현재 HP 값을 표시한다.
	UFUNCTION()
	void OnCurrentHPChange(float InCurrentHP);

protected:
	// 현재 HP를 표시할 TextBlock
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;

	// 최대 HP를 표시할 TextBlock
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;

	// 이 위젯을 소유한 액터
	// 예: AI 캐릭터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;
};