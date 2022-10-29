// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyPlayerController.h"

#include "MyPawn/MyPawn.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("ActionLeft", IE_Pressed, this, &AMyPlayerController::HandleActionLeft);
	InputComponent->BindAction("ActionRight", IE_Pressed, this, &AMyPlayerController::HandleActionRight);
}

void AMyPlayerController::HandleActionLeft()
{
	GetPawn<AMyPawn>()->AccelerateLeft();
}

void AMyPlayerController::HandleActionRight()
{
	GetPawn<AMyPawn>()->AccelerateRight();
}
