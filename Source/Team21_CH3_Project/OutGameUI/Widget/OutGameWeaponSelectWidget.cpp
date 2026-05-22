// OutGameWeaponSelectWidget.cpp
#include "OutGameWeaponSelectWidget.h"
#include "OutGameRootWidget.h"
#include "OutGameUI/Preview/OutGameWeaponPreviewManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "OutGameUI/Controller/OutGamePlayerController.h"

void UOutGameWeaponSelectWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();

	if (IsValid(nextWeaponButton) == true)
		nextWeaponButton->OnClicked.AddUniqueDynamic(this, &ThisClass::UOutGameWeaponSelectWidget::HandleNextClicked);
	if (IsValid(previousWeaponButton) == true)
		previousWeaponButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandlePreviousClicked);
	if (IsValid(applyButton) == true)
		applyButton->OnClicked.AddUniqueDynamic(this, &ThisClass::UOutGameWeaponSelectWidget::HandleApplyClicked);

	bIsWeaponCameraMoving = false;
	previewManagerTag = TEXT("WeaponSelect_PreviewManager");
}

void UOutGameWeaponSelectWidget::EnterWeaponSelect(){
	if (EnterWeaponPreview() == false)
		return;

	bIsWeaponCameraMoving = false;
	SetWeaponInfo();
}

void UOutGameWeaponSelectWidget::UpdateWeaponStatBars(const FOutGameWeaponPreviewData& weaponData)
{
	if (IsValid(damageBar) == true) damageBar->SetPercent(weaponData.weaponDamage / 50.0f);
	if (IsValid(ammoBar) == true) ammoBar->SetPercent(weaponData.ammoCapacity / 30.0f);
	if (IsValid(fireRateBar) == true) fireRateBar->SetPercent(weaponData.firePerMinute / 1000.0f);
	if (IsValid(weaponRangeBar) == true) weaponRangeBar->SetPercent(weaponData.maxAttackRange / 25000.0f);

}

void UOutGameWeaponSelectWidget::HandleNextClicked(){
	if (bIsWeaponCameraMoving == true)
		return;
	UpdateNextWeaponData(true);
	bIsWeaponCameraMoving = true;

	GetWorld()->GetTimerManager().SetTimer(
		weaponCameraMoveTimerHandle,
		this,
		&ThisClass::UnlockWeaponCameraMove,
		0.5f,
		false
	);
}

void UOutGameWeaponSelectWidget::HandlePreviousClicked(){
	if (bIsWeaponCameraMoving == true)
		return;
	UpdateNextWeaponData(false);
	bIsWeaponCameraMoving = true;

	GetWorld()->GetTimerManager().SetTimer(
		weaponCameraMoveTimerHandle,
		this,
		&ThisClass::UnlockWeaponCameraMove,
		0.5f,
		false
	);
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
				RootWidgetInstance->SetHeaderVisible(true);
				RootWidgetInstance->ShowWidget(EOutGameWidgetType::MainMenu);
				PC->SetViewTargetByTag("LobbyCamera", 0.0f);
			});
		}
	}
}

void UOutGameWeaponSelectWidget::NavigateWeapon(int32 direction){
	if (bIsWeaponCameraMoving == true)
		return;

	if (direction > 0)
		HandleNextClicked();
	else
		HandlePreviousClicked();

	bIsWeaponCameraMoving = true;

	GetWorld()->GetTimerManager().SetTimer(
		weaponCameraMoveTimerHandle,
		this,
		&ThisClass::UnlockWeaponCameraMove,
		0.5f,
		false
	);
}

void UOutGameWeaponSelectWidget::RequestBack(){ HandleBackClicked(); }

void UOutGameWeaponSelectWidget::UpdateNextWeaponData(bool bIsNext){
	const bool bChanged = bIsNext ? ShowNextPreviewWeapon() : ShowPreviousPreviewWeapon();

	if (bChanged == false)
		return;
	if (currentWeaponData == nullptr)
		return;

	//Camera logic
	if (AOutGamePlayerController* PC = GetOwningPlayer<AOutGamePlayerController>())
	{
		switch (currentWeaponData->weaponType)
		{
		case EWeaponType::Pistol:
			PC->SetViewTargetByTag("FirstSelectCamera", 0.5f);
			break;
		case EWeaponType::Shotgun:
			PC->SetViewTargetByTag("SecondSelectCamera", 0.5f);
			break;
		case EWeaponType::Rifle:
			PC->SetViewTargetByTag("ThirdSelectCamera", 0.5f);
			break;
		default:
			break;
		}
	}

	SetWeaponInfo();
}

void UOutGameWeaponSelectWidget::UnlockWeaponCameraMove(){ bIsWeaponCameraMoving = false; }

void UOutGameWeaponSelectWidget::SetWeaponInfo(){
	weaponNameText->SetText(currentWeaponData->displayName);
	weaponDamageText->SetText(FText::FromString(FString::Printf(TEXT("%0.f"), currentWeaponData->weaponDamage)));
	ammoCapacityText->SetText(FText::FromString(FString::Printf(TEXT("%d"), currentWeaponData->ammoCapacity)));
	fireRateText->SetText(FText::FromString(FString::Printf(TEXT("%0.fRPM"), currentWeaponData->firePerMinute)));
	weaponRangeText->SetText(FText::FromString(FString::Printf(TEXT("%0.f"), currentWeaponData->maxAttackRange)));
	UpdateWeaponStatBars(*currentWeaponData);
}
