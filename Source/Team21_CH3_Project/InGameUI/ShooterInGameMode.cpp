// ShooterInGameMode.cpp

#include "ShooterInGameMode.h"
#include "InGameUI/InGameHUD.h"
#include "Game/TeamGameInstance.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AShooterInGameMode::AShooterInGameMode()
{
	CurrentWave = 1;
	CurrentWaveKillCount = 0;
	TargetKillCount = 0;

	MaxWave = 3;
	BaseTargetKillCount = 5;
	TargetKillCountIncreasePerWave = 2;

	CurrentGold = 0;
	GoldPerEnemyKill = 10;

	bIsWaveInProgress = false;
	bIsShopOpen = false;
	bIsMatchEnded = false;

	OutGameLevelName = TEXT("OutGameMap");
	EndMatchReturnDelay = 3.0f;
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

	StartWave();
}

void AShooterInGameMode::OnCharacterDied(bool bIsPlayer)
{
	if (bIsMatchEnded)
	{
		return;
	}

	if (bIsPlayer)
	{
		EndMatch(false);
		return;
	}

	HandleEnemyDied();
}

void AShooterInGameMode::StartWave()
{
	if (bIsMatchEnded || bIsWaveInProgress)
	{
		return;
	}

	bIsShopOpen = false;
	bIsWaveInProgress = true;

	CurrentWaveKillCount = 0;
	TargetKillCount = CalculateTargetKillCountForWave(CurrentWave);

	RefreshHUDWaveInfo();

	UE_LOG(LogTemp, Warning, TEXT("StartWave / Wave: %d / TargetKillCount: %d"),
		CurrentWave,
		TargetKillCount
	);

	RequestSpawnWave(CurrentWave, TargetKillCount);
}

void AShooterInGameMode::HandleEnemyDied()
{
	if (bIsMatchEnded || !bIsWaveInProgress)
	{
		return;
	}

	CurrentWaveKillCount++;

	AddGold(GoldPerEnemyKill);
	RefreshHUDWaveInfo();

	UE_LOG(LogTemp, Warning, TEXT("EnemyDied / Wave: %d / Kill: %d / Target: %d / Gold: %d"),
		CurrentWave,
		CurrentWaveKillCount,
		TargetKillCount,
		CurrentGold
	);

	if (CurrentWaveKillCount >= TargetKillCount)
	{
		ClearWave();
	}
}

void AShooterInGameMode::ClearWave()
{
	if (bIsMatchEnded || !bIsWaveInProgress)
	{
		return;
	}

	bIsWaveInProgress = false;

	RefreshHUDWaveInfo();

	UE_LOG(LogTemp, Warning, TEXT("ClearWave / Wave: %d / MaxWave: %d"),
		CurrentWave,
		MaxWave
	);

	if (CurrentWave >= MaxWave)
	{
		EndMatch(true);
		return;
	}

	bIsShopOpen = true;

	RequestOpenShop(CurrentWave, CurrentGold);
}

void AShooterInGameMode::StartNextWave()
{
	if (bIsMatchEnded || bIsWaveInProgress || !bIsShopOpen)
	{
		return;
	}

	bIsShopOpen = false;
	CurrentWave++;

	StartWave();
}

void AShooterInGameMode::EndMatch(bool bPlayerWon)
{
	if (bIsMatchEnded)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndMatch blocked. bIsMatchEnded already true."));
		return;
	}

	bIsMatchEnded = true;
	bIsWaveInProgress = false;
	bIsShopOpen = false;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
		if (MyHUD)
		{
			MyHUD->HideRoundTransitionUI();
			MyHUD->HideHPDangerFeedback();
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

	UE_LOG(LogTemp, Warning, TEXT("EndMatch Called. bPlayerWon: %s / Move To: %s After %.2f seconds"),
		bPlayerWon ? TEXT("true") : TEXT("false"),
		*OutGameLevelName.ToString(),
		EndMatchReturnDelay
	);

	GetWorldTimerManager().ClearTimer(EndMatchReturnTimerHandle);

	GetWorldTimerManager().SetTimer(
		EndMatchReturnTimerHandle,
		this,
		&AShooterInGameMode::HandleEndMatchReturnToOutGame,
		EndMatchReturnDelay,
		false
	);
}

int32 AShooterInGameMode::CalculateTargetKillCountForWave(int32 InWave) const
{
	switch (InWave)
	{
	case 1:
		return 5;

	case 2:
		return 7;

	case 3:
		return 10;

	default:
		return 10;
	}
}

void AShooterInGameMode::AddGold(int32 GoldAmount)
{
	if (GoldAmount <= 0)
	{
		return;
	}

	CurrentGold += GoldAmount;
}

void AShooterInGameMode::RefreshHUDWaveInfo()
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

	MyHUD->RefreshWaveUI(
		CurrentWave,
		CurrentWaveKillCount,
		TargetKillCount,
		CurrentGold
	);
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

void AShooterInGameMode::HandleEndMatchReturnToOutGame()
{
	MoveToOutGameMap();
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

void AShooterInGameMode::CmdKillEnemy()
{
	OnCharacterDied(false);
}

void AShooterInGameMode::CmdClearWave()
{
	ClearWave();
}

void AShooterInGameMode::CmdStartNextWave()
{
	StartNextWave();
}

void AShooterInGameMode::CmdMoveOutGame()
{
	MoveToOutGameMap();
}