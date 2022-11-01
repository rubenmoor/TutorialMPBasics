// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Modes/MyLocalPlayer.h"

#include "MyGISubsystem.generated.h"

#define SETTING_CUSTOMNAME FName(TEXT("CUSTOMNAME"))
#define SETTING_LEVEL FName(TEXT("LEVEL"))

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API UMyGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool CreateSession(const FLocalPlayerContext& LPC, struct FHostSessionConfig SessionConfig, TFunction<void(FName, bool)> Callback);
	void JoinSession(const FLocalPlayerContext& LPC, TFunction<void(ECurrentLevel, EOnJoinSessionCompleteResult::Type)> Callback);

	void LeaveSession();
	
	// show the login browser window for EOS
	void ShowLoginScreen(const FLocalPlayerContext& LPC);
	
protected:
	// event handlers
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	IOnlineSessionPtr GetSessionInterface() const;
};
