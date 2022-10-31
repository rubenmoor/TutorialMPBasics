// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyPlayerController.h"

#include "MyPawn/MyPawn.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	BindAction("ActionLeft", IE_Pressed, EAction::Left);
	BindAction("ActionRight", IE_Pressed, EAction::Right);
}

void AMyPlayerController::HandleAction(EAction Action) const
{
	switch(Action)
	{
	case EAction::Left:
		GetPawn<AMyPawn>()->AccelerateLeft();
		break;
	case EAction::Right:
		GetPawn<AMyPawn>()->AccelerateRight();
		break;
	}
}

void AMyPlayerController::BindAction(const FName ActionName, EInputEvent KeyEvent, EAction Action)
{
	FInputActionBinding Binding(ActionName, KeyEvent);
	if(GetLocalRole() == ROLE_Authority)
	{
		Binding.ActionDelegate.GetDelegateForManualSet().BindLambda([this, Action] ()
		{
			HandleAction(Action);
		});
	}
	else
	{
		Binding.ActionDelegate.GetDelegateForManualSet().BindLambda([this, Action] ()
		{
			ServerRPC_HandleAction(Action);
		});
	}
	InputComponent->AddActionBinding(Binding);
}

void AMyPlayerController::ServerRPC_HandleAction_Implementation(EAction Action)
{
	HandleAction(Action);
}
