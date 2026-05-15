// TeamGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Trait/Data/TraitData.h"
#include "Data/PlayerTraitBonus.h"
#include "TeamGameInstance.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8{
	Rifle = 0 UMETA(DisplayName = "Rifle"),
	Shotgun = 1 UMETA(DisplayName = "Shotgun"),
	Pistol = 2 UMETA(DisplayName = "Pistol"),
	None
};

class UTeamSaveGame;

UCLASS(BlueprintType)
class TEAM21_CH3_PROJECT_API UTeamGameInstance : public UGameInstance{
	GENERATED_BODY()
	
public:
	UTeamGameInstance();
	
	virtual void Init() override;

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Weapon")
	EWeaponType GetSelectedWeaponType() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Weapon")
	void SetSelectedWeaponType(EWeaponType weaponType);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Record")
	int32 GetPlayerTotalKillCount() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Record")
	void SetPlayerTotalKillCount(int32 killCount);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Record")
	void AddPlayerKillCount(int32 killCount = 1);
	
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Result")
	bool GetIsWin() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Result")
	void SetIsWin(bool bNewIsWin);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Result")
	bool GetMatch() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Result")
	void SetMatch(bool bHasMatch);

	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Wave")
	void SaveInGameWaveData(int32 InCurrentWave, int32 InCurrentGold);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Wave")
	void ClearInGameWaveData();
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Wave")
	bool HasSavedInGameWaveData() const;
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Wave")
	int32 GetSavedCurrentWave() const;
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Wave")
	int32 GetSavedCurrentGold() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType selectedWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Record", meta = (AllowPrivateAccess = "true"))
	int32 playerTotalKillCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Result", meta = (AllowPrivateAccess = "true"))
	bool bIsWin;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Team Game Instance|Result", meta = (AllowPrivateAccess = "true"))
	bool bHasMatchResult;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Wave", meta = (AllowPrivateAccess = "true"))
	int32 SavedCurrentWave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Wave", meta = (AllowPrivateAccess = "true"))
	int32 SavedCurrentGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Wave", meta = (AllowPrivateAccess = "true"))
	bool bHasSavedInGameWaveData;
	
#pragma region SaveGame
	
public:
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	float GetMouseSensitivity() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void SetMouseSensitivity(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	float GetMasterVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void SetMasterVolume(float value);
	
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	int32 GetPlayerGold() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void AddPlayerGold(int32 gold);
	
	UFUNCTION()
	void LoadGameData();
	UFUNCTION()
	void SaveGameData();
	UFUNCTION()
	void StartNewGame();
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	int32 playerGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	float mouseSensitivity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	float masterVolume;
	
	UPROPERTY()
	TObjectPtr<UTeamSaveGame> CurrentSaveGame;
	
#pragma endregion
	
#pragma region TraitSystem
	
public:
	int32 GetTraitLevel(FName TraitId) const;
	const TMap<FName, int32>& GetTraitLevels() const;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Trait", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> traitLevels;
	
	UFUNCTION(BlueprintCallable)
	bool CanUpgradeTrait(FName traitId, const FTraitData& traitData) const;
	UFUNCTION(BlueprintCallable)
	bool TryUpgradeTrait(FName traitId, const FTraitData& traitData);
	UFUNCTION(BlueprintCallable)
	bool SpendPlayerGold(int32 Cost);
	FPlayerTraitBonus GetTotalTraitBonus(UDataTable* TraitDataTable) const;
	
#pragma endregion 
};
