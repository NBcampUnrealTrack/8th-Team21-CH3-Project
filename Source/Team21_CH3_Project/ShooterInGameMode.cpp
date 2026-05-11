// ShooterInGameMode.cpp

#include "ShooterInGameMode.h"
#include "InGameUI/InGameHUD.h"
#include "Game/TeamGameInstance.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AShooterInGameMode::AShooterInGameMode()
{
	CurrentRound = 0;
	PlayerScore = 0;
	AIScore = 0;

	bIsRoundTransitioning = false;
	bIsMatchEnded = false;

	RoundTransitionDuration = 2.0f;
	TargetScoreToWin = 3;

	OutGameLevelName = TEXT("OutGameMap");
}

void AShooterInGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputModeData;
		PC->SetInputMode(InputModeData);
		PC->bShowMouseCursor = false;
	}

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (GI)
	{
		bIsMatchEnded = GI->GetMatch();

		if (bIsMatchEnded)
		{
			TriggerResultUI(GI->GetIsWin());
			return;
		}
	}

	StartRound();
}

void AShooterInGameMode::StartRound()
{
	if (bIsRoundTransitioning || bIsMatchEnded)
	{
		return;
	}

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}

	PlayerScore = GI->GetPlayerScore();
	AIScore = GI->GetAIScore();

	CurrentRound = PlayerScore + AIScore + 1;

	RefreshHUDMatchInfo();
}

void AShooterInGameMode::OnCharacterDied(bool bIsPlayer)
{
	if (bIsRoundTransitioning || bIsMatchEnded)
	{
		return;
	}

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}

	const bool bPlayerWonRound = !bIsPlayer;

	if (bPlayerWonRound)
	{
		GI->AddPlayerScore(1);
	}
	else
	{
		GI->AddAIScore(1);
	}

	PlayerScore = GI->GetPlayerScore();
	AIScore = GI->GetAIScore();

	RefreshHUDMatchInfo();

	EndRound(bPlayerWonRound);
}

void AShooterInGameMode::EndRound(bool bPlayerWonRound)
{
	if (bIsMatchEnded)
	{
		return;
	}

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}

	const int32 CurrentPlayerScore = GI->GetPlayerScore();
	const int32 CurrentAIScore = GI->GetAIScore();

	UE_LOG(LogTemp, Warning, TEXT("EndRound / PlayerScore: %d / AIScore: %d / TargetScore: %d"),
		CurrentPlayerScore,
		CurrentAIScore,
		TargetScoreToWin
	);

	const bool bPlayerMatchWin = CurrentPlayerScore >= TargetScoreToWin;
	const bool bAIMatchWin = CurrentAIScore >= TargetScoreToWin;

	if (bPlayerMatchWin)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player reached target score. EndMatch(true)."));
		EndMatch(true);
		return;
	}

	if (bAIMatchWin)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI reached target score. EndMatch(false)."));
		EndMatch(false);
		return;
	}

	bIsRoundTransitioning = true;

	ShowRoundTransitionMessage(bPlayerWonRound);

	GetWorldTimerManager().ClearTimer(RoundTransitionTimerHandle);
	GetWorldTimerManager().SetTimer(
		RoundTransitionTimerHandle,
		this,
		&AShooterInGameMode::FinishRoundTransition,
		RoundTransitionDuration,
		false
	);
}

void AShooterInGameMode::FinishRoundTransition()
{
	if (bIsMatchEnded)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(RoundTransitionTimerHandle);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
		if (MyHUD)
		{
			MyHUD->HideRoundTransitionUI();
		}
	}

	bIsRoundTransitioning = false;

	RestartCurrentLevel();
}

void AShooterInGameMode::EndMatch(bool bPlayerWon)
{
	if (bIsMatchEnded)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndMatch blocked. bIsMatchEnded already true."));
		return;
	}

	bIsMatchEnded = true;
	bIsRoundTransitioning = false;

	GetWorldTimerManager().ClearTimer(RoundTransitionTimerHandle);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
		if (MyHUD)
		{
			MyHUD->HideRoundTransitionUI();
		}
	}

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetIsWin(bPlayerWon);
		GI->SetMatch(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("EndMatch Called. bPlayerWon: %s / Move To: %s"),
		bPlayerWon ? TEXT("true") : TEXT("false"),
		*OutGameLevelName.ToString()
	);

	MoveToOutGameMap();
}

void AShooterInGameMode::StopGameplayInput()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (PlayerPawn)
	{
		PlayerPawn->DisableInput(PC);
	}

	FInputModeUIOnly InputModeData;
	PC->SetInputMode(InputModeData);
	PC->bShowMouseCursor = true;
}

void AShooterInGameMode::RestartCurrentLevel()
{
	if (bIsMatchEnded)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FString CurrentMapName = World->GetMapName();

	const FString StreamingPrefix = World->StreamingLevelsPrefix;
	CurrentMapName.RemoveFromStart(StreamingPrefix);

	UE_LOG(LogTemp, Warning, TEXT("RestartCurrentLevel: %s"), *CurrentMapName);

	UGameplayStatics::OpenLevel(this, FName(*CurrentMapName));
}

void AShooterInGameMode::RefreshHUDMatchInfo()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD)
	{
		return;
	}

	MyHUD->RefreshMatchUI(PlayerScore, AIScore, CurrentRound);
}

void AShooterInGameMode::ShowRoundTransitionMessage(bool bPlayerWonRound)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD)
	{
		return;
	}

	if (MyHUD->IsRoundTransitionUIShowing())
	{
		return;
	}

	const FText MainMessage = bPlayerWonRound
		? FText::FromString(TEXT("Round Win"))
		: FText::FromString(TEXT("Round Lose"));

	const FText SubMessage = FText::FromString(TEXT("Get Ready for the Next Round"));

	MyHUD->ShowRoundTransitionUI(MainMessage, SubMessage);
}

void AShooterInGameMode::MoveToOutGameMap()
{
	if (OutGameLevelName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("OutGameLevelName is None."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MoveToOutGameMap Called. Target Level: %s"), *OutGameLevelName.ToString());

	UGameplayStatics::OpenLevel(this, OutGameLevelName);
}

void AShooterInGameMode::CmdAddPlayerScore()
{
	OnCharacterDied(false);
}

void AShooterInGameMode::CmdAddAIScore()
{
	OnCharacterDied(true);
}

void AShooterInGameMode::CmdMoveOutGame()
{
	MoveToOutGameMap();
}