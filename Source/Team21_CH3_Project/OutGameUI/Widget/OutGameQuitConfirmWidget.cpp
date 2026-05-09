// OutGameQuitConfirmWidget.cpp
#include "OutGameQuitConfirmWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UOutGameQuitConfirmWidget::NativeOnInitialized(){
	Super::NativeOnInitialized();
	
	if (IsValid(confirmButton) == true) confirmButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleConfirmClicked);
	if (IsValid(cancelButton) == true) cancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCancelClicked);
}

void UOutGameQuitConfirmWidget::HandleConfirmClicked(){
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UOutGameQuitConfirmWidget::HandleCancelClicked(){
	SetVisibility(ESlateVisibility::Collapsed);
}
