#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AugmentationDataTable.h" // 아까 만든 데이터 구조 헤더
#include "AugmentComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM21_CH3_PROJECT_API UAugmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAugmentComponent();

protected:
    virtual void BeginPlay() override;

public:
    // --- 시스템 핵심 함수 ---

    /** 위젯에서 카드를 선택했을 때 호출할 함수 */
    UFUNCTION(BlueprintCallable, Category = "Augment")
    void ApplyAugment(EAugmentType Type);

    /** 최종 데미지 보정치를 계산하여 반환 (캐릭터의 공격 로직에서 호출) */
    float GetCalculatedDamage(float InBaseDamage, AActor* Target);

    /** 이동 속도 보정치를 반환 (캐릭터의 속도 설정 로직에서 호출) */
    float GetMoveSpeedModifier() const;


    // --- 이벤트 접점 함수 (캐릭터 담당자가 호출해줘야 함) ---

    /** 적 처치 시 호출 */
    void HandleEnemyKilled(AActor* KilledEnemy);

    /** 재장전 완료 시 호출 */
    void HandleReloadFinished();

    /** 발사 시 호출 */
    void HandleWeaponFired();

    /** 특정 증강의 현재 레벨을 반환 (없으면 0) */
    int32 GetAugmentLevel(EAugmentType Type) const
    {
        return OwnedAugments.Contains(Type) ? OwnedAugments[Type] : 0;
    }

private:
    // --- 내부 관리 데이터 ---

    /** 현재 소유한 증강들과 그 레벨 (중첩 횟수) */
    UPROPERTY(VisibleAnywhere, Category = "Augment|Data")
    TMap<EAugmentType, int32> OwnedAugments;

    /** 데이터 테이블 참조 (수치 계산용) */
    UPROPERTY(EditAnywhere, Category = "Augment|Data")
    UDataTable* AugmentDataTable;

    // --- 상태 변수 (Buff/State) ---

    /** 연속 처치 버프용 타이머 핸들 */
    FTimerHandle ChainKillTimerHandle;
    bool bIsChainKillActive = false;

    /** 장전 보상 활성화 여부 (다음 1발) */
    bool bIsReloadRewardActive = false;

    // --- 수치 계산 보조 함수 ---
    float GetAugmentCurrentValue(EAugmentType Type);
    const FAugmentTableData* GetAugmentData(EAugmentType Type);

    /** 위기 본능 등 상태 업데이트 */
    void CheckCrisisInstinct();
};