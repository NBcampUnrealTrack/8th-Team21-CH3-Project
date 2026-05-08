// OutGameWeaponSelectWidget.cpp
#include "OutGameWeaponSelectWidget.h"
#include "OutGameRootWidget.h"
#include "OutGameUI/Preview/OutGameWeaponPreviewManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"

void UOutGameWeaponSelectWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();

	AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance();
	if (IsValid(previewManager) == false) return;
	
	if (previewManager->ShowWeaponByIndex(0) == false) return;
	currentWeaponData = previewManager->GetCurrentWeaponData();
	
	if (currentWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("currentWeaponData not found"));
		return;
	}

	SetWeaponInfo();
	
	if (IsValid(nextWeaponButton) == true) nextWeaponButton->OnClicked.AddUniqueDynamic(this, &ThisClass::UOutGameWeaponSelectWidget::HandleNextClicked);
	if (IsValid(previousWeaponButton) == true) previousWeaponButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandlePreviousClicked);
	if (IsValid(applyButton) == true) applyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::UOutGameWeaponSelectWidget::HandleApplyClicked);
	if (IsValid(backButton) == true) backButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackClicked);
}

void UOutGameWeaponSelectWidget::HandleNextClicked(){
	UpdateNextWeaponData(true);
}

void UOutGameWeaponSelectWidget::HandlePreviousClicked(){
	UpdateNextWeaponData(false);
}

void UOutGameWeaponSelectWidget::HandleApplyClicked(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* RootWidgetInstance = PC->GetRootWidget())
		{
			RootWidgetInstance->ShowTransition([this, RootWidgetInstance, PC]
			{
				if (UTeamGameInstance* GI = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance()))
				{
					switch (currentWeaponData->weaponType)
					{
					case EWeaponType::Rifle:
						GI->SetSelectedWeaponType(EWeaponType::Rifle);
						break;
					case EWeaponType::Shotgun:
						GI->SetSelectedWeaponType(EWeaponType::Shotgun);
						break;
					case EWeaponType::Pistol:
						GI->SetSelectedWeaponType(EWeaponType::Pistol);
						break;
					default:
					break;
					}
					ClearWeaponPreview();
					RootWidgetInstance->OpenSelectedLevel();
				}
			});
		}
	}
}

void UOutGameWeaponSelectWidget::HandleBackClicked(){
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		if (UOutGameRootWidget* RootWidgetInstance = PC->GetRootWidget())
		{
			RootWidgetInstance->ShowTransition([this, RootWidgetInstance, PC]
			{
				if (IsValid(GetWeaponPreviewManagerInstance()) == true)
				{
					GetWeaponPreviewManagerInstance()->SetWeaponIndex(0);
				}
				ClearWeaponPreview();
				RootWidgetInstance->SetHeaderVisible(true);
				RootWidgetInstance->ShowWidget(EOutGameWidgetType::MainMenu);
				PC->SetViewTargetByTag("LobbyCamera", 0.0f);
			});
		}
	}
}

void UOutGameWeaponSelectWidget::UpdateNextWeaponData(bool bIsNext){
	AOutGameWeaponPreviewManager* previewManager = GetWeaponPreviewManagerInstance();
	if (IsValid(previewManager) == false) return;
	
	const bool bChanged = bIsNext
		? previewManager->ShowNextWeapon()
		: previewManager->ShowPreviousWeapon();

	if (bChanged == false) return;
	
	currentWeaponData = previewManager->GetCurrentWeaponData();
	if (currentWeaponData == nullptr) return;
	
	//Camera logic
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		switch (currentWeaponData->weaponType)
		{
		case EWeaponType::Rifle:
			PC->SetViewTargetByTag("RifleSelectCamera", 0.5f);
			break;
		case EWeaponType::Shotgun:
			PC->SetViewTargetByTag("ShotgunSelectCamera", 0.5f);
			break;
		case EWeaponType::Pistol:
			PC->SetViewTargetByTag("PistolSelectCamera", 0.5f);
			break;
		default:
			break;
		}
	}

	SetWeaponInfo();
}

void UOutGameWeaponSelectWidget::SetWeaponInfo(){
	weaponNameText->SetText(currentWeaponData->displayName);
	weaponDamageText->SetText(FText::FromString(FString::Printf(TEXT("%0.f"), currentWeaponData->weaponDamage)));
	ammoCapacityText->SetText(FText::FromString(FString::Printf(TEXT("%d"), currentWeaponData->ammoCapacity)));
	fireRateText->SetText(FText::FromString(FString::Printf(TEXT("%0.f"), currentWeaponData->firePerMinute)));
	weaponRangeText->SetText(FText::FromString(FString::Printf(TEXT("%0.f"), currentWeaponData->maxAttackRange)));
}

void UOutGameWeaponSelectWidget::ClearWeaponPreview(){
	GetWeaponPreviewManagerInstance()->ClearPreviewWeapon();
}

AOutGameWeaponPreviewManager* UOutGameWeaponSelectWidget::GetWeaponPreviewManagerInstance(){
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(
		this,
		AOutGameWeaponPreviewManager::StaticClass(),
		foundActors
	);

	if (foundActors.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PreviewManager not found"));
		return nullptr;
	}
	
	AOutGameWeaponPreviewManager* PM = Cast<AOutGameWeaponPreviewManager>(foundActors[0]);
	
	if (IsValid(PM) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon not found"));
		return nullptr;
	}
	
	return PM;
}
