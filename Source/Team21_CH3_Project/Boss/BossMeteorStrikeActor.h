// BossMeteorStrikeActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossMeteorStrikeActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UAudioComponent;

UCLASS()
class TEAM21_CH3_PROJECT_API ABossMeteorStrikeActor : public AActor{
	GENERATED_BODY()

public:
	ABossMeteorStrikeActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> sceneRoot;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> warningCircle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Meteor|Niagara", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UNiagaraComponent> meteorNiagara;
	UPROPERTY(EditAnywhere, Category = "Meteor|Impact")
	TObjectPtr<UNiagaraSystem> impactNiagaraSystem;
	UPROPERTY(EditAnywhere, Category = "Meteor|Sound")
	TObjectPtr<USoundBase> impactSound;
	UPROPERTY(EditAnywhere, Category = "Meteor|Sound")
	TObjectPtr<USoundBase> fallingSound;
	UPROPERTY(EditAnywhere, Category = "Meteor|Sound")
	TObjectPtr<UAudioComponent> fallingAudioComponent;
	
	UPROPERTY(EditAnywhere, Category = "Meteor")
	float fillWarningCircleDuration;
	UPROPERTY(EditAnywhere, Category = "Meteor")
	float meteorStartHeight;
	UPROPERTY(EditAnywhere, Category = "Meteor")
	float impactRadius;
	UPROPERTY(EditAnywhere, Category = "Meteor")
	float damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor|Warning", meta = (AllowPrivateAccess = "true"))
	FVector warningStartScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor|Warning", meta = (AllowPrivateAccess = "true"))
	FVector warningTargetScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor|Warning", meta = (AllowPrivateAccess = "true"))
	FVector warningRelativeOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor|Meteor", meta = (AllowPrivateAccess = "true"))
	FVector meteorStartOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meteor|Meteor", meta = (AllowPrivateAccess = "true"))
	FVector meteorEndOffset;

	UFUNCTION()
	void ApplyDamageInRadius();
	
	float elapsedTime;
};
