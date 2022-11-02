// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyPlayerController.h"

#include "Modes/MyGameInstance.h"
#include "Modes/MyGISubsystem.h"
#include "MyPawn/MyPawn.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("ActionEscape", IE_Pressed, GetGameInstance<UMyGameInstance>(), &UMyGameInstance::MulticastRPC_LeaveSession);

	// `BindActionRPC` is a custom private function, see below.
	// Usually you would see here: calls to `InputComponent->BindAction`.
	// This custom function implements a remote procedure call (RPC) to the server.
	// Not all input action affect the game world and thus not all actions imply a RPC,
	// e.g. if a player opens their inventory screen, this would be purely local.
	BindActionWithRPC("ActionLeft", IE_Pressed, EAction::Left);
	BindActionWithRPC("ActionRight", IE_Pressed, EAction::Right);
}

void AMyPlayerController::ClientRPC_LeaveSession_Implementation()
{
	GetGameInstance()->GetSubsystem<UMyGISubsystem>()->LeaveSession();
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

void AMyPlayerController::BindActionWithRPC(const FName ActionName, EInputEvent KeyEvent, EAction Action)
{
	// We use a somewhat complicated way to bind actions.
	// This is necessary though as the conventions `InputComponent->BindAction` doesn't allow to pass arguments
	// to the handler.
	// We use `BindLambda` to have the `EAction Action` parameter via a closure
	FInputActionBinding Binding(ActionName, KeyEvent);
	if(GetLocalRole() == ROLE_Authority)
	{
		// In a listen server setup (as opposed to a dedicated server), the `ROLE_Authority` means:
		// We are the host, in which case we just literally carry out the action.
		// In principle, we still need to take care of the propagation of any relevant data to the connected clients.
		// However, this is where Unreal's replication comes into play. Our actions change some property on some pawn
		// on the host and from there the replication mechanism of the pawn takes over.
		// Se "MyPawn/MyPawn.h" for details.
		Binding.ActionDelegate.GetDelegateForManualSet().BindLambda([this, Action] ()
		{
			HandleAction(Action);
		});
	}
	else
	{
		// If we are not on the host, we use an RPC to make sure our action gets executed there.
		// Note: A local key press (by a connected client) only happens there. This local key press then gets
		// translated to this RPC. I.e. the key press gets sent into the network *without* affecting the local
		// player's pawn directly. Only from there via a replicated property, the consequences of the key press come
		// into effect for the local client.
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
