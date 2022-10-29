// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyGISubsystem.generated.h"

#define SETTING_CUSTOMNAME FName(TEXT("CUSTOMNAME"))

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API UMyGISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool CreateSession(const FLocalPlayerContext& LPC, struct FHostSessionConfig SessionConfig, TFunction<void(FName, bool)> Callback);
	bool JoinSession(const FLocalPlayerContext& LPC, TFunction<void(FName, EOnJoinSessionCompleteResult::Type)> Callback);
	
	// show the login browser window for EOS
	void ShowLoginScreen(const FLocalPlayerContext& LPC);
	
	TArray<FOnlineSessionSearchResult> GetSearchResult() const { return LastSessionSearch->SearchResults; }
	
protected:
	// event handlers
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	IOnlineSessionPtr GetSessionInterface() const;
	
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
};
