// OutGamePlayerController.cpp
#include "OutGameUI/Controller/OutGamePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OutGameUI/Widget/OutGameRootWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/TeamGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

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
	
	FInputModeGameAndUI inputMode;
	inputMode.SetHideCursorDuringCapture(false); // Drag and Click -> don't hide cursor
	SetInputMode(inputMode);
	
	bShowMouseCursor = true;
}

void AOutGamePlayerController::SetupInputComponent(){
	Super::SetupInputComponent();
	
	InputComponent->BindKey(EKeys::A, IE_Pressed, this, &ThisClass::HandleNavigateLeft);
	InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ThisClass::HandleNavigateRight);
	InputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ThisClass::HandleEscPressed);
}

void AOutGamePlayerController::SetViewTargetByTag(FName cameraTag, float blendTime){
	TArray<AActor*> foundCameras;
	UGameplayStatics::GetAllActorsWithTag(this, cameraTag, foundCameras);
	
	if (foundCameras.Num() > 0 && IsValid(foundCameras[0]))
	{
		SetViewTargetWithBlend(foundCameras[0], blendTime,VTBlend_EaseInOut, 2.0f);
	}
}

UOutGameRootWidget* AOutGamePlayerController::GetRootWidget() const{
	if (IsValid(RootWidgetInstance) == true) return RootWidgetInstance;
	return nullptr;
}

void AOutGamePlayerController::HandleNavigateLeft(){
	if (IsValid(RootWidgetInstance) == true) RootWidgetInstance->HandleNavigateHorizontal(-1);
}

void AOutGamePlayerController::HandleNavigateRight(){
	if (IsValid(RootWidgetInstance) == true) RootWidgetInstance->HandleNavigateHorizontal(1);
}

void AOutGamePlayerController::HandleEscPressed(){
	if (IsValid(RootWidgetInstance) == true) RootWidgetInstance->HandleBackRequested();
}
