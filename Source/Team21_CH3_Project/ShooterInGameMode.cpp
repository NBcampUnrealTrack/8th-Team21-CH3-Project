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

	MaxWave = 5;
	BaseTargetKillCount = 5;
	TargetKillCountIncreasePerWave = 2;

	CurrentGold = 0;
	GoldPerEnemyKill = 10;

	bIsWaveInProgress = false;
	bIsShopOpen = false;
	bIsMatchEnded = false;

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

	StartWave();
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

	UE_LOG(LogTemp, Warning, TEXT("StartWave / Wave: %d / TargetKillCount: %d"),
		CurrentWave,
		TargetKillCount
	);

	RefreshHUDWaveInfo();

	// EnemySpawner 실제 C++ 연결 전까지는 BP 이벤트로 스폰 요청을 보낸다.
	RequestSpawnWave(CurrentWave, TargetKillCount);
}

void AShooterInGameMode::OnCharacterDied(bool bIsPlayer)
{
	if (bIsMatchEnded)
	{
		return;
	}

	if (bIsPlayer)
	{
		// 플레이어가 사망하면 즉시 패배 Result 처리
		EndMatch(false);
		return;
	}

	// Enemy 사망
	HandleEnemyDied();
}

void AShooterInGameMode::HandleEnemyDied()
{
	if (bIsMatchEnded || !bIsWaveInProgress)
	{
		return;
	}

	CurrentWaveKillCount++;

	AddGold(GoldPerEnemyKill);

	UE_LOG(LogTemp, Warning, TEXT("EnemyDied / Wave: %d / Kill: %d / Target: %d / Gold: %d"),
		CurrentWave,
		CurrentWaveKillCount,
		TargetKillCount,
		CurrentGold
	);

	RefreshHUDWaveInfo();

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

	UE_LOG(LogTemp, Warning, TEXT("ClearWave / Wave: %d / MaxWave: %d"),
		CurrentWave,
		MaxWave
	);

	RefreshHUDWaveInfo();

	if (CurrentWave >= MaxWave)
	{
		EndMatch(true);
		return;
	}

	bIsShopOpen = true;

	// Shop UI Open 요청
	// 실제 Shop UI 연결은 다음 단계에서 처리
	RequestOpenShop(CurrentWave, CurrentGold);
}

void AShooterInGameMode::StartNextWave()
{
	if (bIsMatchEnded || bIsWaveInProgress)
	{
		return;
	}

	if (!bIsShopOpen)
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

	UE_LOG(LogTemp, Warning, TEXT("EndMatch Called. bPlayerWon: %s / Move To: %s"),
		bPlayerWon ? TEXT("true") : TEXT("false"),
		*OutGameLevelName.ToString()
	);

	MoveToOutGameMap();
}

int32 AShooterInGameMode::CalculateTargetKillCountForWave(int32 InWave) const
{
	const int32 SafeWave = FMath::Max(1, InWave);

	return BaseTargetKillCount + ((SafeWave - 1) * TargetKillCountIncreasePerWave);
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