// OutGamePlayerController.cpp
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/TeamGameInstance.h"

void AOutGamePlayerController::BeginPlay(){
	Super::BeginPlay();
	
	if (!IsLocalController()) return;
	
	SetViewTargetByTag("LobbyCamera", 0.0f);
	
	if (IsValid(RootWidgetClass) == true)
	{
		RootWidgetInstance = CreateWidget<UOutGameRootWidget>(this, RootWidgetClass);
		if (IsValid(RootWidgetInstance) == true)
		{
			RootWidgetInstance->AddToViewport();
			UTeamGameInstance* gameInstance = Cast<UTeamGameInstance>(GetWorld()->GetGameInstance());
			if (IsValid(gameInstance) == true)
			{
				if (gameInstance->GetMatch() == true)
				{
					RootWidgetInstance->ShowTransitionFadein();
					RootWidgetInstance->ShowWidget(EOutGameWidgetType::Result);
					// gameInstance->SetMatch(false);
				}
			}
		}
	}
	
	bShowMouseCursor = true;
	
	FInputModeUIOnly inputMode;
	SetInputMode(inputMode);
}

UOutGameRootWidget* AOutGamePlayerController::GetRootWidget() const{
	if (IsValid(RootWidgetInstance) == true) return RootWidgetInstance;
	return nullptr;
}

void AOutGamePlayerController::SetViewTargetByTag(FName cameraTag, float blendTime){
	TArray<AActor*> foundCameras;
	UGameplayStatics::GetAllActorsWithTag(this, cameraTag, foundCameras);
	
	if (foundCameras.Num() > 0 && IsValid(foundCameras[0]))
	{
		SetViewTargetWithBlend(foundCameras[0], blendTime);
	}
}
