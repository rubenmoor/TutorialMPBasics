// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

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

protected:
	void HandleAction(EAction Action) const;

private:
	// bind an action from the EAction enum and do the appropriate RPC
	void BindAction(const FName ActionName, EInputEvent KeyEvent, EAction Action);

	UFUNCTION(Server, Reliable)
	void ServerRPC_HandleAction(EAction Action);
};
