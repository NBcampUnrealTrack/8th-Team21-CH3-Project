// TeamHPTextWidgetComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TeamHPTextWidgetComponent.generated.h"

// AI/캐릭터 머리 위에 HP Text 위젯을 띄우기 위한 WidgetComponent
UCLASS()
class TEAM21_CH3_PROJECT_API UTeamHPTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UTeamHPTextWidgetComponent();

	// WidgetComponent가 실제 UserWidget을 생성한 뒤 호출되는 함수
	virtual void InitWidget() override;
};