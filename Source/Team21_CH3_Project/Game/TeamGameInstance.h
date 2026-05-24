// TeamGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Data/AugmentationDataTable.h"
#include "TeamGameInstance.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8{
	Rifle = 0 UMETA(DisplayName = "Rifle"),
	Shotgun = 1 UMETA(DisplayName = "Shotgun"),
	Pistol = 2 UMETA(DisplayName = "Pistol"),
	None
};

class UTeamSaveGame;
class USoundClass;
class USoundMix;

UCLASS(BlueprintType)
class TEAM21_CH3_PROJECT_API UTeamGameInstance : public UGameInstance{
	GENERATED_BODY()
	
public:
	UTeamGameInstance();
	
	virtual void Init() override;

	void ResetToDefaultValues();
	
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Weapon")
	EWeaponType GetSelectedWeaponType() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Weapon")
	void SetSelectedWeaponType(EWeaponType weaponType);
	
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
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Wave")
	float GetCurrentHp() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Wave")
	void SetCurrentHp(float currentHp);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType selectedWeaponType;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Wave", meta = (AllowPrivateAccess = "true"))
	float currentPlayerHp;
	
#pragma region SaveGame
	
public:
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Record")
	int32 GetPlayerTotalKillCount() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Record")
	void SetPlayerTotalKillCount(int32 killCount);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Record")
	void AddPlayerKillCount(int32 killCount = 1);
	
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	float GetMouseSensitivity() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void SetMouseSensitivity(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	int32 GetPlayerGold() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void AddPlayerGold(int32 gold);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void SetGoldGainMultiplier(float goldGainMultiplier);
	
	UFUNCTION()
	void LoadGameData();
	UFUNCTION()
	void SaveGameData();
	UFUNCTION()
	void StartNewGame();
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Record", meta = (AllowPrivateAccess = "true"))
	int32 playerTotalKillCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	int32 playerGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	float mouseSensitivity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	float goldGainMultiplier = 1.0f;
	
	UPROPERTY()
	TObjectPtr<UTeamSaveGame> CurrentSaveGame;
	
#pragma endregion

#pragma region Sound

public:
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Sound")
	float GetMasterVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Sound")
	void SetMasterVolume(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Sound")
	float GetBGMVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Sound")
	void SetBGMVolume(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Sound")
	float GetSFXVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Sound")
	void SetSFXVolume(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Sound")
	float GetUIVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Sound")
	void SetUIVolume(float value);

	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Sound")
	void ApplySoundSettings();

private:
	void ApplySoundClassVolume(USoundClass* soundClass, float volume);

	UPROPERTY(EditDefaultsOnly, Category = "Team Game Instance|Sound")
	TObjectPtr<USoundMix> soundMix;
	UPROPERTY(EditDefaultsOnly, Category = "Team Game Instance|Sound")
	TObjectPtr<USoundClass> masterSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "Team Game Instance|Sound")
	TObjectPtr<USoundClass> bgmSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "Team Game Instance|Sound")
	TObjectPtr<USoundClass> sfxSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "Team Game Instance|Sound")
	TObjectPtr<USoundClass> uiSoundClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Sound", meta = (AllowPrivateAccess = "true"))
	float masterVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Sound", meta = (AllowPrivateAccess = "true"))
	float bgmVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Sound", meta = (AllowPrivateAccess = "true"))
	float sfxVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Sound", meta = (AllowPrivateAccess = "true"))
	float uiVolume;

#pragma endregion
	
#pragma region TraitSystem
	
public:
	int32 GetTraitLevel(FName TraitId) const;
	const TMap<FName, int32>& GetTraitLevels() const;
	
	UFUNCTION(BlueprintCallable)
	bool SpendPlayerGold(int32 Cost);
	UFUNCTION(BlueprintCallable)
	void TraitLevelUp(FName traitId);
	UFUNCTION(BlueprintCallable)
	void ApplyGoldTraitBonus();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team Game Instance|Trait", meta = (AllowPrivateAccess))
	TObjectPtr<UDataTable> traitDataTable;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Trait", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> traitLevels;
	
#pragma endregion 

#pragma region AugmentationSaveLoad

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Augment Save Data")
	TMap<EAugmentType, int32> SavedAugments;

	void SaveCharacterAugments(const TMap<EAugmentType, int32>& InAugments);

	TMap<EAugmentType, int32> LoadCharacterAugments() const;

	void ClearSavedAugments();

#pragma endregion
	
#pragma region ToolTip
	
public:
	bool HasShownKeyGuide() const;
	void SetHasShownKeyGuide(bool bInShown);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Guide", meta = (AllowPrivateAccess))
	bool bHasShownKeyGuide;
	
#pragma endregion 
};
