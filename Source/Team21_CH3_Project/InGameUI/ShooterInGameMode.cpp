// ShooterInGameMode.cpp

#include "ShooterInGameMode.h"
#include "InGameUI/InGameHUD.h"
#include "InGameUI/AugmentCardSelectWidget.h"
#include "Game/TeamGameInstance.h"
#include "Data/EnemyWaveDataTable.h"
#include "Component/StatusComponent.h"
#include "OutGameUI/Widget/OutGameTransitionWidget.h"
#include "GameFramework/Character.h"

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
	WaveLevelTransitionDelay = 1.0f;

	OutGameLevelName = TEXT("OutGameMap");
	EndMatchReturnDelay = 3.0f;
	EndMatchTransitionDelay = 1.0f;

	HUDWaveRefreshRetryCount = 0;
	MaxHUDWaveRefreshRetryCount = 10;
	HUDWaveRefreshRetryInterval = 0.05f;

	AugmentKillInterval = 5;
	bIsAugmentSelectOpen = false;
	bPendingClearWaveAfterAugment = false;

	OutGameTransitionWidgetInstance = nullptr;

	BossStatusComponentForUI = nullptr;
	BossMaxHPForUI = 1.0f;
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

	GetWorldTimerManager().ClearTimer(GameStartWaveTimerHandle);
	GetWorldTimerManager().SetTimer(
		GameStartWaveTimerHandle,
		this,
		&AShooterInGameMode::StartWave,
		GameStartTransitionDelay,
		false
	);

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

	// Boss Wave일 때 Boss Actor가 스폰된 뒤 HP Bar를 찾는다.
	if (CurrentWave == BossWaveIndex)
	{
		BossStatusComponentForUI = nullptr;
		BossMaxHPForUI = 1.0f;

		GetWorldTimerManager().ClearTimer(BossHPBarFindTimerHandle);
		GetWorldTimerManager().ClearTimer(BossHPBarUpdateTimerHandle);

		GetWorldTimerManager().SetTimer(
			BossHPBarFindTimerHandle,
			this,
			&AShooterInGameMode::TryShowBossHPBar,
			0.2f,
			true
		);
	}
	else
	{
		HideBossHPBar();
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

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->AddPlayerKillCount(ClearedWaveKillCount);
	}

	bIsWaveInProgress = false;

	HideBossHPBar();

	RefreshHUDWaveInfo();

	UE_LOG(LogTemp, Warning, TEXT("ClearWave / Wave: %d / KillCount: %d / MaxWave: %d"),
		CurrentWave,
		ClearedWaveKillCount,
		MaxWave
	);

	// 마지막 웨이브는 보스 사망 연출이 멈추지 않도록 Pause를 걸지 않는다.
	if (CurrentWave >= MaxWave)
	{
		EndMatch(true);
		return;
	}

	// 마지막 웨이브가 아닐 때만 Wave Clear Pause 적용
	StartWaveClearPause();

	bIsShopOpen = true;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (IsValid(PlayerCharacter))
	{
		UFunction* ForceStopFullAutoFireFunction = PlayerCharacter->FindFunction(TEXT("ForceStopFullAutoFire"));

		if (ForceStopFullAutoFireFunction)
		{
			PlayerCharacter->ProcessEvent(ForceStopFullAutoFireFunction, nullptr);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ForceStopFullAutoFire function not found on PlayerCharacter."));
		}
	}

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

void AShooterInGameMode::StartWaveClearPause()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ClearWaveClearPauseTicker();

	if (WaveClearPauseDuration <= 0.0f)
	{
		RestoreWaveClearPause();
		return;
	}

	UGameplayStatics::SetGamePaused(World, true);

	WaveClearPauseTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(
			this,
			&AShooterInGameMode::HandleWaveClearPauseFinished
		),
		WaveClearPauseDuration
	);
}

bool AShooterInGameMode::HandleWaveClearPauseFinished(float DeltaTime)
{
	RestoreWaveClearPause();

	WaveClearPauseTickerHandle.Reset();

	return false;
}

void AShooterInGameMode::RestoreWaveClearPause()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(World, false);
}

void AShooterInGameMode::ClearWaveClearPauseTicker()
{
	if (WaveClearPauseTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(WaveClearPauseTickerHandle);
		WaveClearPauseTickerHandle.Reset();
	}
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

	HideBossHPBar();

	SavePlayerHPToGameInstance();

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (PC && PC->GetPawn())
	{
		UAugmentComponent* AugmentComp = PC->GetPawn()->FindComponentByClass<UAugmentComponent>();
		if (AugmentComp)
		{
			AugmentComp->SaveToGameInstance();
		}
	}

	UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SaveInGameWaveData(CurrentWave, CurrentGold);
	}

	UE_LOG(LogTemp, Warning, TEXT("ContinueToNextWaveWithLevelReload / NextWave: %d / Gold: %d"),
		CurrentWave,
		CurrentGold
	);

	if (PC)
	{
		AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
		if (MyHUD)
		{
			MyHUD->PlayLevelTransitionFadeOut();
		}
	}

	GetWorldTimerManager().ClearTimer(WaveLevelTransitionTimerHandle);

	GetWorldTimerManager().SetTimer(
		WaveLevelTransitionTimerHandle,
		this,
		&AShooterInGameMode::ReloadCurrentLevel,
		WaveLevelTransitionDelay,
		false
	);
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

	GetWorldTimerManager().ClearTimer(GameStartWaveTimerHandle);
	GetWorldTimerManager().ClearTimer(NextWaveStartTimerHandle);
	GetWorldTimerManager().ClearTimer(WaveLevelTransitionTimerHandle);
	GetWorldTimerManager().ClearTimer(BossHPBarFindTimerHandle);
	GetWorldTimerManager().ClearTimer(BossHPBarUpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(EndMatchReturnTimerHandle);
	GetWorldTimerManager().ClearTimer(EndMatchTransitionTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDWaveRefreshRetryTimerHandle);

	ClearWaveClearPauseTicker();

	RestoreWaveClearPause();

	HideBossHPBar();
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

		GI->SaveInGameWaveData(CurrentWave, FinalGold);
		GI->ClearInGameWaveData();
		GI->SetCurrentHp(0.0f);

		GI->ClearSavedAugments();
	}

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

void AShooterInGameMode::TryShowBossHPBar()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> FoundBossActors;
	UGameplayStatics::GetAllActorsWithTag(
		World,
		BossActorTag,
		FoundBossActors
	);

	if (FoundBossActors.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss actor not found yet. Tag: %s"), *BossActorTag.ToString());
		return;
	}

	AActor* BossActor = FoundBossActors[0];
	if (!IsValid(BossActor))
	{
		return;
	}

	UStatusComponent* BossStatusComponent = nullptr;

	TArray<UStatusComponent*> StatusComponents;
	BossActor->GetComponents<UStatusComponent>(StatusComponents);

	float HighestCurrentHP = -1.0f;

	for (UStatusComponent* StatusComp : StatusComponents)
	{
		if (!IsValid(StatusComp))
		{
			continue;
		}

		const float CurrentHP = StatusComp->GetCurrentHP();

		UE_LOG(LogTemp, Warning, TEXT("Boss Status Candidate / Name: %s / Class: %s / CurrentHP: %.2f"),
			*StatusComp->GetName(),
			*StatusComp->GetClass()->GetName(),
			CurrentHP
		);

		if (CurrentHP > HighestCurrentHP)
		{
			HighestCurrentHP = CurrentHP;
			BossStatusComponent = StatusComp;
		}
	}

	if (!IsValid(BossStatusComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss StatusComponent not found."));
		return;
	}

	BossStatusComponentForUI = BossStatusComponent;
	BossMaxHPForUI = FMath::Max(1.0f, BossStatusComponent->GetCurrentHP());

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD)
	{
		return;
	}

	MyHUD->ShowBossHPBar();

	GetWorldTimerManager().ClearTimer(BossHPBarFindTimerHandle);
	GetWorldTimerManager().ClearTimer(BossHPBarUpdateTimerHandle);

	GetWorldTimerManager().SetTimer(
		BossHPBarUpdateTimerHandle,
		this,
		&AShooterInGameMode::UpdateBossHPBarByTimer,
		0.05f,
		true
	);

	UpdateBossHPBarByTimer();

	UE_LOG(LogTemp, Warning, TEXT("Boss HP Bar Shown. Boss: %s / BoundComponent: %s / MaxHPForUI: %.2f"),
		*BossActor->GetName(),
		*BossStatusComponent->GetName(),
		BossMaxHPForUI
	);
}

void AShooterInGameMode::UpdateBossHPBarByTimer()
{
	if (!IsValid(BossStatusComponentForUI))
	{
		HideBossHPBar();
		return;
	}

	const float CurrentHP = FMath::Max(0.0f, BossStatusComponentForUI->GetCurrentHP());
	const float MaxHP = FMath::Max(1.0f, BossMaxHPForUI);

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return;
	}

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD)
	{
		return;
	}

	MyHUD->UpdateBossHPBar(CurrentHP, MaxHP);

	if (CurrentHP <= 0.0f)
	{
		HideBossHPBar();
	}
}

void AShooterInGameMode::HideBossHPBar()
{
	GetWorldTimerManager().ClearTimer(BossHPBarFindTimerHandle);
	GetWorldTimerManager().ClearTimer(BossHPBarUpdateTimerHandle);

	BossStatusComponentForUI = nullptr;
	BossMaxHPForUI = 1.0f;

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return;
	}

	AInGameHUD* MyHUD = Cast<AInGameHUD>(PC->GetHUD());
	if (!MyHUD)
	{
		return;
	}

	MyHUD->HideBossHPBar();
}

void AShooterInGameMode::ShowAugmentCardSelectUI()
{
	if (bIsMatchEnded || bIsAugmentSelectOpen) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	UAugmentComponent* AugmentComp = PlayerPawn->FindComponentByClass<UAugmentComponent>();
	if (!AugmentComp) return;

	bIsAugmentSelectOpen = true;

	PC->SetPause(true);
	StopGameplayInput();

	AugmentComp->AugmentSelection();

	if (UAugmentCardSelectWidget* ActiveWidget = AugmentComp->GetAugmentWidget())
	{
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(ActiveWidget->TakeWidget());
		PC->SetInputMode(InputModeData);
	}
}

void AShooterInGameMode::HideAugmentCardSelectUI()
{
	bIsAugmentSelectOpen = false;
}

void AShooterInGameMode::NotifyAugmentSelectionComplete()
{
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
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		MoveToOutGameMap();
		return;
	}

	if (!OutGameTransitionWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("OutGameTransitionWidgetClass is not set. MoveToOutGameMap directly."));
		MoveToOutGameMap();
		return;
	}

	if (!OutGameTransitionWidgetInstance)
	{
		OutGameTransitionWidgetInstance = CreateWidget<UOutGameTransitionWidget>(
			PC,
			OutGameTransitionWidgetClass
		);

		if (OutGameTransitionWidgetInstance)
		{
			OutGameTransitionWidgetInstance->AddToViewport(10000);
		}
	}

	if (!OutGameTransitionWidgetInstance)
	{
		MoveToOutGameMap();
		return;
	}

	OutGameTransitionWidgetInstance->PlayFadeOut();

	GetWorldTimerManager().ClearTimer(EndMatchTransitionTimerHandle);

	GetWorldTimerManager().SetTimer(
		EndMatchTransitionTimerHandle,
		this,
		&AShooterInGameMode::MoveToOutGameMap,
		EndMatchTransitionDelay,
		false
	);
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

void AShooterInGameMode::CmdDamageBoss()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> FoundBossActors;
	UGameplayStatics::GetAllActorsWithTag(
		World,
		BossActorTag,
		FoundBossActors
	);

	if (FoundBossActors.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("CmdDamageBoss Failed. Boss actor not found. Tag: %s"), *BossActorTag.ToString());
		return;
	}

	AActor* BossActor = FoundBossActors[0];
	if (!IsValid(BossActor))
	{
		return;
	}

	UStatusComponent* BossStatusComponent = nullptr;

	TArray<UStatusComponent*> StatusComponents;
	BossActor->GetComponents<UStatusComponent>(StatusComponents);

	float HighestCurrentHP = -1.0f;

	for (UStatusComponent* StatusComp : StatusComponents)
	{
		if (!IsValid(StatusComp))
		{
			continue;
		}

		const float CurrentHP = StatusComp->GetCurrentHP();

		if (CurrentHP > HighestCurrentHP)
		{
			HighestCurrentHP = CurrentHP;
			BossStatusComponent = StatusComp;
		}
	}

	if (!IsValid(BossStatusComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("CmdDamageBoss Failed. Boss StatusComponent not found."));
		return;
	}

	const float DamageAmount = 9999.0f;
	const float CurrentHP = BossStatusComponent->GetCurrentHP();
	const float NewHP = FMath::Max(0.0f, CurrentHP - DamageAmount);

	BossStatusComponent->SetCurrentHP(NewHP);

	UE_LOG(LogTemp, Warning, TEXT("CmdDamageBoss / Boss: %s / Damage: %.2f / HP: %.2f -> %.2f"),
		*BossActor->GetName(),
		DamageAmount,
		CurrentHP,
		NewHP
	);
}