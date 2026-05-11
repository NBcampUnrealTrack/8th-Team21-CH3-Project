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
			StopGameplayInput();
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
	if (!GI) return;

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
	if (!GI) return;

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
	if (!GI) return;

	const bool bPlayerMatchWin = GI->GetPlayerScore() >= TargetScoreToWin;
	const bool bAIMatchWin = GI->GetAIScore() >= TargetScoreToWin;

	if (bPlayerMatchWin)
	{
		EndMatch(true);
		return;
	}

	if (bAIMatchWin)
	{
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

	StopGameplayInput();

	TriggerResultUI(bPlayerWon);

	if (bPlayerWon)
	{
		UE_LOG(LogTemp, Warning, TEXT("MATCH END: PLAYER WINS!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MATCH END: AI WINS!"));
	}
}

void AShooterInGameMode::StopGameplayInput()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

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
	if (!World) return;

	FString CurrentMapName = World->GetMapName();

	const FString StreamingPrefix = World->StreamingLevelsPrefix;
	CurrentMapName.RemoveFromStart(StreamingPrefix);

	UGameplayStatics::OpenLevel(this, FName(*CurrentMapName));
}

void AShooterInGameMode::RefreshHUDMatchInfo()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD) return;

	MyHUD->RefreshMatchUI(PlayerScore, AIScore, CurrentRound);
}

void AShooterInGameMode::ShowRoundTransitionMessage(bool bPlayerWonRound)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD) return;

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

void AShooterInGameMode::CmdAddPlayerScore()
{
	OnCharacterDied(false);
}

void AShooterInGameMode::CmdAddAIScore()
{
	OnCharacterDied(true);
}