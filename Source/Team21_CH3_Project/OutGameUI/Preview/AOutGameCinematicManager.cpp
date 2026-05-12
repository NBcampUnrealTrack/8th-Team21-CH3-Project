// AOutGameCinematicManager.cpp
#include "OutGameUI/Preview/AOutGameCinematicManager.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"

void AAOutGameCinematicManager::PlayResultCinematic(bool bIsWin){
	ULevelSequence* sequenceToPlay = bIsWin ? victorySequence : defeatSequence;
	
	if (IsValid(sequenceToPlay) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sequence Is Valid"));
		OnCinematicFinished.Broadcast();
		return;
	}
	
	FMovieSceneSequencePlaybackSettings playbackSettings;
	playbackSettings.FinishCompletionStateOverride =
		EMovieSceneCompletionModeOverride::ForceKeepState;
	
	ALevelSequenceActor* createdSequenceActor = nullptr;
	
	sequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(),
		sequenceToPlay,
		playbackSettings,
		createdSequenceActor
	);
	
	sequenceActor = createdSequenceActor;
	
	if (IsValid(sequencePlayer) == false)
	{
		OnCinematicFinished.Broadcast();
		return;
	}
	
	sequencePlayer->OnFinished.AddDynamic(this, &ThisClass::HandleSequenceFinished);
	sequencePlayer->Play();
}

void AAOutGameCinematicManager::HandleSequenceFinished(){
	OnCinematicFinished.Broadcast();
	
}


