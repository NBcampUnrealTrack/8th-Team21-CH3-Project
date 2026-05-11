// AOutGameCinematicManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOutGameCinematicManager.generated.h"

class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCinematicFinished);

UCLASS()
class TEAM21_CH3_PROJECT_API AAOutGameCinematicManager : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION()
	void PlayResultCinematic(bool bIsWin);
	
	UPROPERTY()
	FOnCinematicFinished OnCinematicFinished;
protected:
	UPROPERTY(EditAnywhere, Category = "Cinematic")
	TObjectPtr<ULevelSequence> victorySequence;
	UPROPERTY(EditAnywhere, Category = "Cinematic")
	TObjectPtr<ULevelSequence> defeatSequence;
	
private:
	UPROPERTY()
	TObjectPtr<ULevelSequencePlayer> sequencePlayer;
	UPROPERTY()
	TObjectPtr<ALevelSequenceActor> sequenceActor;
	
	UFUNCTION()
	void HandleSequenceFinished();
	
};
