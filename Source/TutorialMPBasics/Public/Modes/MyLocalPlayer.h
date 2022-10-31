// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "MyLocalPlayer.generated.h"

/*
 * an enum to make explicit where this player currently is
 */
UENUM(BlueprintType)
enum class ECurrentLevel : uint8
{
	MainMenu       = 0 UMETA(DisplayName="main menu"),
	SomeLevel      = 1 UMETA(DisplayName="some level"),
	SomeOtherLevel = 2 UMETA(DisplayName="some other level")
};

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API UMyLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
	
public:
	// a convenient way to check whether this player is currently inside the main menu
	UFUNCTION(BlueprintCallable)
	bool GetIsInMainMenu()
	{
		return CurrentLevel == ECurrentLevel::MainMenu;
	}

	// application state specific to the player

	// the player is either inside the main menu or some level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECurrentLevel CurrentLevel;

	// if true inside the main menu: hosting or joining a multiplayer game
	// if true inside a level: playing a multiplayer game
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsMultiplayer;

	// only has meaning when inside some level: whether or not the in-game menu is currently shown
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool ShowInGameMenu;

	// the logged-in state regarding some online subsystem (e.g. EOS or Steam, not NULL/LAN)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsLoggedIn;
};
