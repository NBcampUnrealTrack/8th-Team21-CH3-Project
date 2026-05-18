// ShooterInGameMode.cpp

#include "ShooterInGameMode.h"
#include "InGameUI/InGameHUD.h"
#include "InGameUI/AugmentCardSelectWidget.h"
#include "Game/TeamGameInstance.h"
#include "Data/EnemyWaveDataTable.h"
#include "Component/StatusComponent.h"

#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/SpawnManager.h"

#include "Component/AugmentComponent.h"

AShooterInGameMode::AShooterInGameMode()
{
	WaveDataTable = nullptr;

	CurrentWave = 1;
	CurrentWaveKillCount = 0;
	TargetKillCount = 0;

	MaxWave = 4;
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

	ActiveWidget = nullptr;
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

	RestorePlayerHPFromGameInstance();

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
	GoldPerEnemyKill = CalculateGoldPerKillForWave(CurrentWave);

	bIsAugmentSelectOpen = false;
	bPendingClearWaveAfterAugment = false;

	RefreshHUDWaveInfo();
	RequestHUDWaveInfoRefreshRetry();

	UE_LOG(LogTemp, Warning, TEXT("StartWave / Wave: %d / TargetKillCount: %d / GoldPerKill: %d"),
		CurrentWave,
		TargetKillCount,
		GoldPerEnemyKill
	);

	if (!TryStartWaveWithSpawnManager(CurrentWave))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager not found. Fallback to RequestSpawnWave."));

		RequestSpawnWave(CurrentWave, TargetKillCount);
	}
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

	UE_LOG(LogTemp, Warning, TEXT("EnemyDied / Wave: %d / Kill: %d / Target: %d / Gold: %d / GoldPerKill: %d"),
		CurrentWave,
		CurrentWaveKillCount,
		TargetKillCount,
		CurrentGold,
		GoldPerEnemyKill
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

	SavePlayerHPToGameInstance();

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

		// 다음 게임 시작 시 이전 인게임 HP가 남지 않도록 초기화
		GI->SetCurrentHp(0.0f);
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

FName AShooterInGameMode::MakeWaveDataRowName(int32 InWave) const
{
	const FString RowNameString = FString::Printf(TEXT("Wave_%02d"), InWave);
	return FName(*RowNameString);
}

int32 AShooterInGameMode::CalculateTargetKillCountForWave(int32 InWave) const
{
	if (WaveDataTable)
	{
		const FName RowName = MakeWaveDataRowName(InWave);
		const FEnemyWaveDataTable* WaveData = WaveDataTable->FindRow<FEnemyWaveDataTable>(RowName, TEXT("CalculateTargetKillCountForWave"));

		if (WaveData)
		{
			const int32 TotalEnemyCount =
				FMath::Max(0, WaveData->NormalCount) +
				FMath::Max(0, WaveData->RusherCount) +
				FMath::Max(0, WaveData->ShooterCount) +
				FMath::Max(0, WaveData->BossCount);

			if (TotalEnemyCount > 0)
			{
				return TotalEnemyCount;
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("WaveDataTable row not found or enemy count is 0. Row: %s"),
			*RowName.ToString()
		);
	}

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

int32 AShooterInGameMode::CalculateGoldPerKillForWave(int32 InWave) const
{
	if (WaveDataTable)
	{
		const FName RowName = MakeWaveDataRowName(InWave);
		const FEnemyWaveDataTable* WaveData = WaveDataTable->FindRow<FEnemyWaveDataTable>(RowName, TEXT("CalculateGoldPerKillForWave"));

		if (WaveData)
		{
			return FMath::Max(0, WaveData->GoldPerKill);
		}

		UE_LOG(LogTemp, Warning, TEXT("WaveDataTable row not found. Row: %s"),
			*RowName.ToString()
		);
	}

	return GoldPerEnemyKill;
}

bool AShooterInGameMode::TryStartWaveWithSpawnManager(int32 InWave)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	TArray<AActor*> FoundSpawnManagers;
	UGameplayStatics::GetAllActorsOfClass(
		World,
		ASpawnManager::StaticClass(),
		FoundSpawnManagers
	);

	if (FoundSpawnManagers.Num() <= 0)
	{
		return false;
	}

	ASpawnManager* SpawnManager = Cast<ASpawnManager>(FoundSpawnManagers[0]);
	if (!SpawnManager)
	{
		return false;
	}

	SpawnManager->StartWave(InWave);

	UE_LOG(LogTemp, Warning, TEXT("SpawnManager StartWave Called / Wave: %d"), InWave);

	return true;
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
		CurrentGold,
		GoldPerEnemyKill
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

	// 1. 플레이어 컨트롤러를 통해 플레이어 캐릭터를 가져옵니다.
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	// 2. 캐릭터가 들고 있는 AugmentComponent를 찾습니다.
	UAugmentComponent* AugmentComp = PlayerPawn->FindComponentByClass<UAugmentComponent>();
	if (!AugmentComp)
	{
		UE_LOG(LogTemp, Error, TEXT("ShowAugmentCardSelectUI: AugmentComponent not found on Player!"));
		return;
	}

	// 3. 게임모드가 플래그만 세워두고, 실제 위젯 띄우기 노가다는 컴포넌트에게 전권을 넘깁니다!
	bIsAugmentSelectOpen = true;
	AugmentComp->AugmentSelection();
}

void AShooterInGameMode::HideAugmentCardSelectUI()
{
	if (ActiveWidget)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}

	bIsAugmentSelectOpen = false;
}

void AShooterInGameMode::HandleAugmentSelected(FAugmentResult SelectedCardData)
{
	// UE_LOG(LogTemp, Warning, TEXT("Augment Selected"));

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

UStatusComponent* AShooterInGameMode::GetPlayerStatusComponent() const
{
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return nullptr;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		return nullptr;
	}

	return PlayerPawn->FindComponentByClass<UStatusComponent>();
}

void AShooterInGameMode::SavePlayerHPToGameInstance()
{
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}

	UStatusComponent* PlayerStatusComponent = GetPlayerStatusComponent();
	if (!PlayerStatusComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SavePlayerHPToGameInstance Failed. Player StatusComponent not found."));
		return;
	}

	const float CurrentHP = PlayerStatusComponent->GetCurrentHP();
	GI->SetCurrentHp(CurrentHP);

	UE_LOG(LogTemp, Warning, TEXT("Save Player HP / HP: %.2f"), CurrentHP);
}

void AShooterInGameMode::RestorePlayerHPFromGameInstance()
{
	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (!GI)
	{
		return;
	}

	const float SavedHP = GI->GetCurrentHp();

	if (SavedHP <= 0.0f)
	{
		return;
	}

	UStatusComponent* PlayerStatusComponent = GetPlayerStatusComponent();
	if (!PlayerStatusComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("RestorePlayerHPFromGameInstance Failed. Player StatusComponent not found."));
		return;
	}

	PlayerStatusComponent->SetCurrentHP(SavedHP);

	UE_LOG(LogTemp, Warning, TEXT("Restore Player HP / HP: %.2f"), SavedHP);
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

void AShooterInGameMode::EnemyKilled(AActor* KilledEnemy)
{
	if (OnEnemyKilledDelegate.IsBound())
	{
		OnEnemyKilledDelegate.Broadcast(KilledEnemy);
	}
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