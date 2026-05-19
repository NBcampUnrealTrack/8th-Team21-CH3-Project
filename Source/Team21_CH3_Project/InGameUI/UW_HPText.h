// UW_HPText.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HPText.generated.h"

class UStatusComponent;
class UTextBlock;
class UProgressBar;

// AI 머리 위에 표시할 HP UI 위젯
// StatusComponent의 HP 변경 델리게이트를 받아 HPBar / Text를 갱신한다.
UCLASS()
class TEAM21_CH3_PROJECT_API UUW_HPText : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_HPText(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void BeginDestroy() override;

	// HP 정보를 가진 StatusComponent를 받아서 HP UI를 초기화하고 델리게이트를 연결한다.
	void InitializeHPTextWidget(UStatusComponent* InStatusComponent);

	AActor* GetOwningActor() const { return OwningActor; }
	void SetOwningActor(AActor* InOwningActor) { OwningActor = InOwningActor; }

	void OnMaxHPChange(float InMaxHP);
	void OnCurrentHPChange(float InCurrentHP);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UStatusComponent> BoundStatusComponent;

private:
	// StatusComponent의 현재 값을 기준으로 HPBar와 HP Text를 갱신한다.
	void UpdateHPDisplay();

	// StatusComponent 델리게이트에 연결한다.
	void BindStatusDelegates();

	// StatusComponent 델리게이트 연결을 해제한다.
	void UnbindStatusDelegates();

	// 중복 바인딩 방지용 플래그
	bool bIsStatusDelegateBound;
};