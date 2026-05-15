// ShooterInGameMode.cpp

#include "ShooterInGameMode.h"
#include "InGameUI/InGameHUD.h"
#include "InGameUI/AugmentCardSelectWidget.h"
#include "Game/TeamGameInstance.h"

#include "Blueprint/UserWidget.h"
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

	NextWaveStartDelay = 3.0f;

	OutGameLevelName = TEXT("OutGameMap");
	EndMatchReturnDelay = 3.0f;

	HUDWaveRefreshRetryCount = 0;
	MaxHUDWaveRefreshRetryCount = 10;
	HUDWaveRefreshRetryInterval = 0.05f;

	ActiveAugmentCardSelectWidget = nullptr;
	AugmentKillInterval = 5;
	bIsAugmentSelectOpen = false;
	bPendingClearWaveAfterAugment = false;
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

		if (GI->HasSavedInGameWaveData())
		{
			CurrentWave = GI->GetSavedCurrentWave();
			CurrentGold = GI->GetSavedCurrentGold();

			UE_LOG(LogTemp, Warning, TEXT("Restore Wave Data / Wave: %d / Gold: %d"),
				CurrentWave,
				CurrentGold
			);
		}
	}

	StartWave();

	RequestHUDWaveInfoRefreshRetry();
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

	bIsAugmentSelectOpen = false;
	bPendingClearWaveAfterAugment = false;

	RefreshHUDWaveInfo();
	RequestHUDWaveInfoRefreshRetry();

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

	if (bIsAugmentSelectOpen)
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

	const bool bShouldClearWave = CurrentWaveKillCount >= TargetKillCount;
	const bool bShouldShowAugment =
		AugmentKillInterval > 0 &&
		CurrentWaveKillCount > 0 &&
		CurrentWaveKillCount % AugmentKillInterval == 0;

	if (bShouldShowAugment)
	{
		bPendingClearWaveAfterAugment = bShouldClearWave;
		ShowAugmentCardSelectUI();
		return;
	}

	if (bShouldClearWave)
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

	const int32 ClearedWaveKillCount = CurrentWaveKillCount;

	// Wave가 끝날 때마다 해당 Wave의 KillCount를 TeamGameInstance에 누적 저장
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->AddPlayerKillCount(ClearedWaveKillCount);
	}

	bIsWaveInProgress = false;

	RefreshHUDWaveInfo();

	UE_LOG(LogTemp, Warning, TEXT("ClearWave / Wave: %d / KillCount: %d / MaxWave: %d"),
		CurrentWave,
		ClearedWaveKillCount,
		MaxWave
	);

	if (CurrentWave >= MaxWave)
	{
		EndMatch(true);
		return;
	}

	bIsShopOpen = true;

	StopGameplayInput();

	TriggerRoundResultUI(CurrentWave, CurrentGold);

	UE_LOG(LogTemp, Warning, TEXT("RoundResult UI shown. Next wave reload after %.2f seconds"),
		NextWaveStartDelay
	);

	GetWorldTimerManager().ClearTimer(NextWaveStartTimerHandle);

	GetWorldTimerManager().SetTimer(
		NextWaveStartTimerHandle,
		this,
		&AShooterInGameMode::HandleAutoStartNextWaveWithLevelReload,
		NextWaveStartDelay,
		false
	);
}
void AShooterInGameMode::StartNextWave()
{
	if (bIsMatchEnded || bIsWaveInProgress || !bIsShopOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNextWave Blocked / MatchEnded: %s / WaveInProgress: %s / ShopOpen: %s"),
			bIsMatchEnded ? TEXT("true") : TEXT("false"),
			bIsWaveInProgress ? TEXT("true") : TEXT("false"),
			bIsShopOpen ? TEXT("true") : TEXT("false")
		);
		return;
	}

	bIsShopOpen = false;
	CurrentWave++;

	StartWave();
}

void AShooterInGameMode::HandleAutoStartNextWaveWithLevelReload()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleAutoStartNextWaveWithLevelReload Called"));

	ContinueToNextWaveWithLevelReload();
}

void AShooterInGameMode::ContinueToNextWaveWithLevelReload()
{
	if (bIsMatchEnded || !bIsShopOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("ContinueToNextWaveWithLevelReload Blocked / MatchEnded: %s / ShopOpen: %s"),
			bIsMatchEnded ? TEXT("true") : TEXT("false"),
			bIsShopOpen ? TEXT("true") : TEXT("false")
		);
		return;
	}

	CurrentWave++;
	CurrentWaveKillCount = 0;

	bIsWaveInProgress = false;
	bIsShopOpen = false;

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SaveInGameWaveData(CurrentWave, CurrentGold);
	}

	UE_LOG(LogTemp, Warning, TEXT("ContinueToNextWaveWithLevelReload / NextWave: %d / Gold: %d"),
		CurrentWave,
		CurrentGold
	);

	ReloadCurrentLevel();
}

void AShooterInGameMode::EndMatch(bool bPlayerWon)
{
	if (bIsMatchEnded)
	{
		UE_LOG(LogTemp, Warning, TEXT("EndMatch blocked. bIsMatchEnded already true."));
		return;
	}

	const int32 FinalWaveKillCount = CurrentWaveKillCount;
	const int32 FinalGold = CurrentGold;

	bIsMatchEnded = true;
	bIsWaveInProgress = false;
	bIsShopOpen = false;

	GetWorldTimerManager().ClearTimer(NextWaveStartTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDWaveRefreshRetryTimerHandle);

	HideAugmentCardSelectUI();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetPause(false);

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

		// 게임 종료 직전 마지막 Wave / Gold 값을 한 번 저장한다.
		// ClearInGameWaveData() 내부에서 SavedCurrentGold를 playerGold에 더하고 저장함.
		GI->SaveInGameWaveData(CurrentWave, FinalGold);

		// 인게임 웨이브 복구용 임시 데이터 초기화
		GI->ClearInGameWaveData();
	}

	// GameMode 내부 값 초기화
	CurrentWaveKillCount = 0;
	CurrentGold = 0;

	StopGameplayInput();

	TriggerResultUI(bPlayerWon);

	UE_LOG(LogTemp, Warning, TEXT("EndMatch Called. bPlayerWon: %s / FinalWave: %d / FinalKillCount: %d / FinalGold: %d / Move To: %s After %.2f seconds"),
		bPlayerWon ? TEXT("true") : TEXT("false"),
		CurrentWave,
		FinalWaveKillCount,
		FinalGold,
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
	TryRefreshHUDWaveInfo();
}

bool AShooterInGameMode::TryRefreshHUDWaveInfo()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return false;
	}

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD)
	{
		return false;
	}

	MyHUD->RefreshWaveUI(
		CurrentWave,
		CurrentWaveKillCount,
		TargetKillCount,
		CurrentGold
	);

	return true;
}

void AShooterInGameMode::RequestHUDWaveInfoRefreshRetry()
{
	HUDWaveRefreshRetryCount = 0;

	GetWorldTimerManager().ClearTimer(HUDWaveRefreshRetryTimerHandle);

	HandleHUDWaveInfoRefreshRetry();
}

void AShooterInGameMode::HandleHUDWaveInfoRefreshRetry()
{
	if (TryRefreshHUDWaveInfo())
	{
		GetWorldTimerManager().ClearTimer(HUDWaveRefreshRetryTimerHandle);
		return;
	}

	HUDWaveRefreshRetryCount++;

	if (HUDWaveRefreshRetryCount >= MaxHUDWaveRefreshRetryCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD Wave Refresh Retry Failed."));
		return;
	}

	GetWorldTimerManager().SetTimer(
		HUDWaveRefreshRetryTimerHandle,
		this,
		&AShooterInGameMode::HandleHUDWaveInfoRefreshRetry,
		HUDWaveRefreshRetryInterval,
		false
	);
}

void AShooterInGameMode::ShowAugmentCardSelectUI()
{
	if (bIsMatchEnded || bIsAugmentSelectOpen)
	{
		return;
	}

	if (!AugmentCardSelectWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("AugmentCardSelectWidgetClass is not set."));
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	ActiveAugmentCardSelectWidget = CreateWidget<UAugmentCardSelectWidget>(PC, AugmentCardSelectWidgetClass);
	if (!ActiveAugmentCardSelectWidget)
	{
		return;
	}

	ActiveAugmentCardSelectWidget->OnAugmentSelected.RemoveDynamic(this, &AShooterInGameMode::HandleAugmentSelected);
	ActiveAugmentCardSelectWidget->OnAugmentSelected.AddDynamic(this, &AShooterInGameMode::HandleAugmentSelected);

	ActiveAugmentCardSelectWidget->AddToViewport(200);

	bIsAugmentSelectOpen = true;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(ActiveAugmentCardSelectWidget->TakeWidget());
	PC->SetInputMode(InputModeData);
	PC->bShowMouseCursor = true;
	PC->SetPause(true);

	UE_LOG(LogTemp, Warning, TEXT("Augment Card Select UI Opened."));
}

void AShooterInGameMode::HideAugmentCardSelectUI()
{
	if (ActiveAugmentCardSelectWidget)
	{
		ActiveAugmentCardSelectWidget->RemoveFromParent();
		ActiveAugmentCardSelectWidget = nullptr;
	}

	bIsAugmentSelectOpen = false;
}

void AShooterInGameMode::HandleAugmentSelected(FAugmentCardData SelectedCardData)
{
	UE_LOG(LogTemp, Warning, TEXT("Augment Selected: %s"),
		*SelectedCardData.CardName.ToString()
	);

	HideAugmentCardSelectUI();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetPause(false);
	}

	ResumeGameplayInput();

	const bool bShouldClearWave = bPendingClearWaveAfterAugment;
	bPendingClearWaveAfterAugment = false;

	if (bShouldClearWave)
	{
		ClearWave();
	}
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

void AShooterInGameMode::ResumeGameplayInput()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (PlayerPawn)
	{
		PlayerPawn->EnableInput(PC);
	}

	FInputModeGameOnly InputModeData;
	PC->SetInputMode(InputModeData);
	PC->bShowMouseCursor = false;
}

void AShooterInGameMode::ReloadCurrentLevel()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(World, true));

	UE_LOG(LogTemp, Warning, TEXT("ReloadCurrentLevel Called. Level: %s"), *CurrentLevelName.ToString());

	UGameplayStatics::OpenLevel(this, CurrentLevelName);
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