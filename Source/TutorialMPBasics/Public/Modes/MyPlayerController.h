// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/*
 * EAction: any action that implies a server RPC is encoded here
 */
UENUM(BlueprintType)
enum class EAction : uint8
{
	Left,
	Right
};

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void SetupInputComponent() override;
	
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_LeaveSession();
protected:
	// locally carry out an `EAction`
	void HandleAction(EAction Action) const;

private:
	// bind an action and do the appropriate RPC, the action needs to be represented in the enum `EAction`
	void BindActionWithRPC(const FName ActionName, EInputEvent KeyEvent, EAction Action);

	// a simple wrapper around `HandleAction`, lifting it to the host, where it then gets executed locally
	UFUNCTION(Server, Reliable)
	void ServerRPC_HandleAction(EAction Action);
};
