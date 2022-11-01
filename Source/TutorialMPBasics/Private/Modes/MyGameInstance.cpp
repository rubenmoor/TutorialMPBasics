// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyGameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Modes/MyGISubsystem.h"
#include "Modes/MyLocalPlayer.h"

void UMyGameInstance::HostGame(const FLocalPlayerContext& LPC)
{
	UMyGISubsystem* GISub = GetSubsystem<UMyGISubsystem>();

	GISub->CreateSession
		( LPC
		, SessionConfig
		// Using a closure here has several advantages:
		// * we can see the code that gets executed asynchronously (the callback) right here, which helps with reading
		//   the program
		// * we don't have to concern ourselves with delegates at all, leaving all that to the
		//   session related code in the Game Instance subsystem
		// * we can pass the local player context `LPC` into the closure, thus we know which local player created the
		//   session and thus has to update their current level; keeping track of this would otherwise require an extra
		//   variable in the game instance
		, [this, LPC] (FName SessionName, bool bSuccess)
		{
			if(bSuccess)
			{
				Cast<UMyLocalPlayer>(LPC.GetLocalPlayer())->CurrentLevel = ECurrentLevel::SomeLevel;
				GetWorld()->ServerTravel("/Game/SomeLevel?listen");
			}
			else
			{
				// This log output is the bare minimum; properly working with error state implies some sort messaging
				// system in our user interface
				UE_LOG(LogNet, Error, TEXT("%s: create session call returned with failure"), *GetFullName())
			}
		});
	
}

void UMyGameInstance::JoinGame(const FLocalPlayerContext& LPC)
{
	Cast<UMyLocalPlayer>(LPC.GetLocalPlayer())->IsMultiplayer = true;
	GetSubsystem<UMyGISubsystem>()->JoinSession(LPC, [this, LPC] (ECurrentLevel NewLevel, EOnJoinSessionCompleteResult::Type Result)
	{
		switch(Result)
		{
		using namespace EOnJoinSessionCompleteResult;
		case SessionIsFull:
			UE_LOG(LogNet, Error, TEXT("%s: Join session: session is full"), *GetFullName())
			break;
		case SessionDoesNotExist:
			UE_LOG(LogNet, Error, TEXT("%s: Join session: session does not exist"), *GetFullName())
			break;
		case CouldNotRetrieveAddress:
			UE_LOG(LogNet, Error, TEXT("%s: Join session: could not retrieve address"), *GetFullName())
			break;
		case AlreadyInSession:
			UE_LOG(LogNet, Error, TEXT("%s: Join session: alreayd in session"), *GetFullName())
			break;
		case UnknownError:
			UE_LOG(LogNet, Error, TEXT("%s: Join session: unknown error"), *GetFullName())
			break;
		case Success:
			if(ClientTravelToSession(LPC.GetLocalPlayer()->GetControllerId(), NAME_GameSession))
			{
				
				Cast<UMyLocalPlayer>(LPC.GetLocalPlayer())->CurrentLevel = NewLevel;
			}
			else
			{
				UE_LOG(LogNet, Error, TEXT("%s: travel to session failed"), *GetFullName())
			}
			break;
		default: ;
		}
	});
}

void UMyGameInstance::LeaveGame_Implementation()
{
	GetSubsystem<UMyGISubsystem>()->LeaveSession();
}

int32 UMyGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, int32 ControllerId)
{
	int32 InsertIndex = Super::AddLocalPlayer(NewPlayer, ControllerId);
	UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(NewPlayer);
	LocalPlayer->CurrentLevel = ECurrentLevel::MainMenu;
	LocalPlayer->IsMultiplayer = false;
	LocalPlayer->ShowInGameMenu = false;

	/*
	 * properly dealing with the logged-in state in an online subsystem like EOS or steam
	 * requires to save the respective unique net id, load it on start up and then running
	 
	LocalPlayer->SetCachedUniqueNetId( // loaded from file // );
	const IOnlineIdentityPtr OSSIdentity = Online::GetIdentityInterfaceChecked(FName(TEXT("EOS")));
	LocalPlayer->IsLoggedIn = OSSIdentity->GetLoginStatus(LocalPlayer->GetCachedUniqueNetId()) == ELoginStatus::LoggedIn;

	 * to check whether we are logged in (e.g. with the EOS subsystem).
	 *
	 * Note that it would be possible to create a unique net id here, but it isn't recommended.
	 * Rather leave that to the respective online subsystem. Even NULL (the online subsystem for LAN) creates
	 * valid unique net ids for you. Those unique net ids carry information of the respective online subsytem with them
	 * as their "type".
	 * 
	 */
	 
	LocalPlayer->IsLoggedIn = false;
	
	return InsertIndex;
}
