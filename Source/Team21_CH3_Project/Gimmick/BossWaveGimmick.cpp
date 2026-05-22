#include "Gimmick/BossWaveGimmick.h"
#include "Character/CharacterBase.h"
#include "Component/StatusComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/NonPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


ABossWaveGimmick::ABossWaveGimmick()
{

	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	GlowEffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GlowEffect"));
	GlowEffectComp->SetupAttachment(RootComponent);

	bIsActivation = false;
	ObjectMaxHP = 200.0f;
	ObjectCurrentHP = ObjectMaxHP;

}



void ABossWaveGimmick::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetVisibility(false);
	GlowEffectComp->SetVisibility(false);

}

void ABossWaveGimmick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// [레벨 리로드 구원 투수] TargetBoss 주소가 없다면, 기믹이 직접 월드에서 보스를 찾아냅니다!
	if (!IsValid(TargetBoss))
	{
		// 월드에 있는 모든 ANonPlayerCharacter(또는 원래 쓰시던 보스 클래스)를 검색
		TArray<AActor*> FoundBosses;
		UGameplayStatics::GetAllActorsOfClass(this, ANonPlayerCharacter::StaticClass(), FoundBosses);

		for (AActor* Actor : FoundBosses)
		{
			// 혹시 보스 구분을 태그로 하신다면 아래 조건문을 켜세요 (로그에 Tag: Boss가 있으므로)
			if (Actor && Actor->ActorHasTag(TEXT("Boss")))
			{
				TargetBoss = Cast<ACharacterBase>(Actor);
				if (IsValid(TargetBoss))
				{
					UE_LOG(LogTemp, Error, TEXT("[GIMMICK SELF-SCAN SUCCESS] Found Boss via World Scan: %s"), *TargetBoss->GetName());
					break;
				}
			}
		}
	}

	// STEP 2 & 3: 보스 주소를 확보했으니 도청 장치(델리게이트) 연결
	if (IsValid(TargetBoss) && !bIsHPBindingSuccess)
	{
		if (GetStatus())
		{
			GetStatus()->OnCurrentHPChanged.RemoveAll(this);
			GetStatus()->OnCurrentHPChanged.AddUObject(this, &ThisClass::HandleBossCurrentHPChanged);

			bIsHPBindingSuccess = true;
			UE_LOG(LogTemp, Error, TEXT("[★ BINDING SUCCESS ★] Gimmick directly hooked into Boss HP!"));
		}
	}

	// Heal Logic (기존 코드 유지)
	if (bIsActivation && IsValid(TargetBoss))
	{
		UStatusComponent* BossStatus = TargetBoss->GetComponentByClass<UStatusComponent>();
		if (IsValid(BossStatus))
		{
			float HealAmount = 10.f * DeltaSeconds;
			float NewHP = FMath::Clamp(BossStatus->GetCurrentHP() + HealAmount, 0.f, BossStatus->GetMaxHP());
			BossStatus->SetCurrentHP(NewHP);
		}
	}
}


void ABossWaveGimmick::ActivateGimmickObject()
{
	if (bIsActivation) return;

	bIsActivation = true;
	ObjectCurrentHP = ObjectMaxHP;

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetVisibility(true);
	GlowEffectComp->SetVisibility(true);

	UE_LOG(LogTemp, Warning, TEXT("BossWaveGimmick Object Activated!"));
}


float ABossWaveGimmick::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("!!! TakeDamage Called !!! Damage: %f, bIsActivation: %d"), DamageAmount, bIsActivation);
	if (!bIsActivation) return 0.0f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ObjectCurrentHP = FMath::Clamp(ObjectCurrentHP - ActualDamage, 0.0f, ObjectMaxHP);

	UE_LOG(LogTemp, Log, TEXT("Gimmick Object HP: %.1f / %.1f"), ObjectCurrentHP, ObjectMaxHP);

	if (ObjectCurrentHP <= 0.0f)
	{
		bIsActivation = false;

		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GlowEffectComp->SetVisibility(false);

		MeshComp->SetVisibility(false);

		if (OnObjectBreaked.IsBound())
		{
			OnObjectBreaked.Broadcast();
		}
	}

	return ActualDamage;
}

void ABossWaveGimmick::SetTargetBoss(ACharacterBase* NewBoss)
{
	if (IsValid(NewBoss))
	{
		TargetBoss = NewBoss;
		UE_LOG(LogTemp, Log, TEXT("[Gimmick] TargetBoss is Successed: %s"), *TargetBoss->GetName());
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[!!! ERROR !!!] SetTargetBoss called, but NewBoss parameter is NULL!"));
	}
}

UStatusComponent* ABossWaveGimmick::GetStatus() {
	ANonPlayerCharacter* Boss = Cast<ANonPlayerCharacter>(TargetBoss);

	if (IsValid(Boss) == false) return nullptr;

	UStatusComponent* status = Boss->GetStatusComponent();
	if (IsValid(status) == false) return nullptr;

	return status;
}


void ABossWaveGimmick::HandleBossCurrentHPChanged(float CurrentHP)
{
	// 이미 켜진 상태라면 계산할 필요 없이 통과
	if (bIsActivation) return;

	if (GetStatus() == nullptr) return;

	const float MaxHP = GetStatus()->GetMaxHP();
	const float Ratio = MaxHP > 0.0f ? CurrentHP / MaxHP : 0.0f;

	// 실시간 테스트용 로그 출력 (소수점 없이 깔끔하게 보려고 %.0f 사용)
	UE_LOG(LogTemp, Log, TEXT("[Gimmick-Test] Boss HP: %.0f / %.0f (Ratio: %.2f)"), CurrentHP, MaxHP, Ratio);

	// ◀ [핵심] 보스 체력이 30% (0.3f) 이하가 되는 순간!
	if (Ratio <= 0.3f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Gimmick-Test] Boss HP is 30%% or less! Activating Gimmick!"));
		ActivateGimmickObject();
	}
}