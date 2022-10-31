// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/*
 * 
 * examples for different rule sets
 * You might expect this definition inside some GameMode class, however, we need it already to configure
 * a game (be it single or multiplayer) in the main menu
 */
UENUM(BlueprintType)
enum class EGameMode : uint8
{
	EveryManForHimself UMETA(DisplayName="every man for himself"),
	Teams UMETA(DisplayName="teams"),
	Coop UMETA(DisplayName="coop"),
};

/*
 * minimal session setup information
 *
 */
USTRUCT(BlueprintType)
struct FHostSessionConfig
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CustomName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NumMaxPlayers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bPrivate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bEnableLAN;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EGameMode GameMode;
};

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void HostGame(const FLocalPlayerContext& LPC);

	// overriding `JoinSession` instead of our own custom definition is possible, too; it doesn't make a big difference,
	// because `Super::JoinSession` doesn't do anything; it does, however, enforce a type signature that doesn't work
	// well for this example
	void JoinGame(const FLocalPlayerContext& LPC);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FHostSessionConfig SessionConfig =
		{ ""
		, 4
		, false
		, true
		, EGameMode::EveryManForHimself
		};
	
protected:
	// event handlers
	void HandleSessionCreated(FName SessionName, bool bSuccess);

	// Using Local Player for player-specific application state requires some initialization we will do here
	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, int32 ControllerId) override;
};
