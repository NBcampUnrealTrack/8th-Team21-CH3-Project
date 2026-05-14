// TeamGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	float GetMouseSensitivity() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void SetMouseSensitivity(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Settings")
	float GetMasterVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Settings")
	void SetMasterVolume(float value);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Record")
	int32 GetPlayerTotalKillCount() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Record")
	void SetPlayerTotalKillCount(int32 killCount);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Record")
	void AddPlayerKillCount(int32 killCount = 1);

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Score")
	int32 GetPlayerScore() const;
	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Score")
	int32 GetAIScore() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Score")
	void AddPlayerScore(int32 score = 1);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Score")
	void AddAIScore(int32 score = 1);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Score")
	void ClearScore();

	UFUNCTION(BlueprintPure, Category = "Team Game Instance|Result")
	bool GetIsWin() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Result")
	void SetIsWin(bool bNewIsWin);
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Result")
	bool GetMatch() const;
	UFUNCTION(BlueprintCallable, Category = "Team Game Instance|Result")
	void SetMatch(bool bHasMatch);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Weapon", meta = (AllowPrivateAccess = "true"))
	EWeaponType selectedWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	float mouseSensitivity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Settings", meta = (AllowPrivateAccess = "true"))
	float masterVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Record", meta = (AllowPrivateAccess = "true"))
	int32 playerTotalKillCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Score", meta = (AllowPrivateAccess = "true"))
	int32 playerScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Score", meta = (AllowPrivateAccess = "true"))
	int32 aiScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team Game Instance|Result", meta = (AllowPrivateAccess = "true"))
	bool bIsWin;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Team Game Instance|Result", meta = (AllowPrivateAccess = "true"))
	bool bHasMatchResult;
	
#pragma region SaveGame
	
public:
	UFUNCTION()
	void LoadGameData();
	UFUNCTION()
	void SaveGameData();
	UFUNCTION()
	void StartNewGame();
	
private:
	UPROPERTY()
	TObjectPtr<UTeamSaveGame> CurrentSaveGame;
	
#pragma endregion
};
