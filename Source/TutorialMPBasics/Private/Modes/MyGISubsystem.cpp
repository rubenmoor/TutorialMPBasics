// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyGISubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "MainMenu/HUD_MainMenu.h"

#include "Modes/MyGameInstance.h"
#include "Modes/MyLocalPlayer.h"

bool UMyGISubsystem::CreateSession(const FLocalPlayerContext& LPC, FHostSessionConfig SessionConfig,
                                   TFunction<void(FName, bool)> Callback)
{
	const IOnlineSessionPtr SI = GetSessionInterface();

	// syntax for unpacking of structs
	auto [ CustomName, NumConnections, bPrivate, bEnableLAN, _GameMode ] = SessionConfig;
	
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPrivateConnections = bPrivate ? NumConnections : 0;
	LastSessionSettings->NumPublicConnections = !bPrivate ? NumConnections : 0;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = bEnableLAN;
	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set(SETTING_MAPNAME, FString(TEXT("some level")), EOnlineDataAdvertisementType::ViaOnlineService);

	// the custom name for the session, provided by the user
	LastSessionSettings->Set(SETTING_CUSTOMNAME, CustomName, EOnlineDataAdvertisementType::ViaOnlineService);

	// As we are dealing with a multicast delegate, we can add as many delegats (= handlers) as we want.
	// By clearing, we make sure to react only once to this event
	if(SI->OnCreateSessionCompleteDelegates.IsBound())
	{
		// We expect this delegate to be unbound the first time the user creates a session.
		// If it isn't, we get a warning.
		UE_LOG(LogNet, Warning, TEXT("%s: OnCreateSessionCompleteDelegates: was bound, clearing"), *GetFullName())
		SI->OnCreateSessionCompleteDelegates.Clear();
	}

	/*
	 * this uses `AddLambda` in combination with a closure that is passed as the function parameter `Callback`,
	 * see above; if you don't like closures, you can equally well use

	SI->OnCreateSessionCompleteDelegates.AddUObject(GetGameInstance(), &UMyGameInstance::HandleCreateSessionComplete);

	 * ... you find the code for the event handler inside MyGameInstance.cpp in the comment at the very bottom.
	 */
	SI->OnCreateSessionCompleteDelegates.AddLambda(Callback);

	/*
	 * Maybe you have seen code like this in some tutorial:

	return SI->CreateSession(*LPC.GetLocalPlayer()->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);

	 * However, the unique net id might be null at this point. By passing the local player index, a unique net id
	 * will be created for us without problem.
	 * Note that often, instead of `GetLocalPlayer()->GetIndexInGameInstance()` just `0` is provided.
	 * The 0 means that the session is always created in the name of the local player with index 0.
	 * This is always safe, as split/shared screen isn't usually possible at the same time as LAN/online multiplayer.
	 * As long as there is only one local player (not split/shared screen), its index is 0.
	 */
	return SI->CreateSession(LPC.GetLocalPlayer()->GetIndexInGameInstance(), NAME_GameSession, *LastSessionSettings);
}

bool UMyGISubsystem::JoinSession(const FLocalPlayerContext& LPC, TFunction<void(FName, EOnJoinSessionCompleteResult::Type)> Callback)
{
	const IOnlineSessionPtr SI = GetSessionInterface();
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = 10000;
	LastSessionSearch->bIsLanQuery = Cast<UMyGameInstance>(GetGameInstance())->SessionConfig.bEnableLAN;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	if(SI->OnFindSessionsCompleteDelegates.IsBound())
	{
		UE_LOG(LogNet, Warning, TEXT("%s: OnFindSessionsCompleteDelegates: was bound, clearing"), *GetFullName())
		SI->OnFindSessionsCompleteDelegates.Clear();
	}
	SI->OnFindSessionsCompleteDelegates.AddLambda([this, LPC, Callback, SI] (bool bSuccess)
	{
		// In case we find a session, we just join immediately;
		// more thoroughly, you make a list of available sessions with their respective custom name and offer the player
		// to join a specific one
		const TArray<FOnlineSessionSearchResult> Results = LastSessionSearch->SearchResults;
		if(bSuccess && !Results.IsEmpty())
		{
			if(SI->OnJoinSessionCompleteDelegates.IsBound())
			{
				UE_LOG(LogNet, Warning, TEXT("%s: OnJoinSessionCompleteDelegates: was bound, clearing"), *GetFullName())
				SI->OnJoinSessionCompleteDelegates.Clear();
			}
			// We are inside a closure that gets executed when "FindSessionsComplete" fires, thus this line means:
			// When we found a session ...
			// ... and when we joined a session ...
			// ... execute `Callback(Fname, EJoinSessionCompleteResult::Type)`
			SI->OnJoinSessionCompleteDelegates.AddLambda(Callback);
			SI->JoinSession(LPC.GetLocalPlayer()->GetIndexInGameInstance(), NAME_GameSession, Results[0]);
		}
		else
		{
			UE_LOG(LogNet, Error, TEXT("%s: couldn't find session."), *GetFullName())
		}
	});

	// after having registered the callback (`AddLambda`) for the FindSessionCompleteEvent, we go and find sessions
	return SI->FindSessions
		(LPC.GetLocalPlayer()->GetIndexInGameInstance()
		, LastSessionSearch.ToSharedRef()
		);
}

void UMyGISubsystem::ShowLoginScreen(const FLocalPlayerContext& LPC)
{
	FOnlineAccountCredentials OnlineAccountCredentials;
	
	// for epic games account
	//OnlineAccountCredentials.Type = "AccountPortal";

	// for DevAuthTool https://dev.epicgames.com/docs/epic-account-services/developer-authentication-tool
	OnlineAccountCredentials.Type = "Developer";
	OnlineAccountCredentials.Id = "localhost:1234";
	OnlineAccountCredentials.Token = "foo";

	const IOnlineIdentityPtr OSSIdentity = Online::GetIdentityInterfaceChecked(FName(TEXT("EOS")));
	
	OSSIdentity->Login
		( LPC.GetLocalPlayer()->GetLocalPlayerIndex()
		, OnlineAccountCredentials
		);
	
	// TODO: autologin for PIE
	
	// TODO: show some sort of loading screen: "logging in ..."
}

void UMyGISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const IOnlineIdentityPtr OSSIdentity = Online::GetIdentityInterface(GetWorld(), FName(TEXT("EOS")));
	if(!OSSIdentity.IsValid())
	{
		UE_LOG
			( LogNet
			, Error
			, TEXT("%s: couldn't get identity interface for EOS. Probably not configured.")
			, *GetFullName()
			)
		return;
	}
	//const IOnlineIdentityPtr OSSIdentity = Online::GetIdentityInterfaceChecked(FName(TEXT("EOS")));
	
	// login handler
	
	if(OSSIdentity->OnLoginCompleteDelegates->IsBound())
	{
		UE_LOG(LogNet, Warning, TEXT("%s: OnLoginCompleteDelegates: was bound, clearing"), *GetFullName())
		OSSIdentity->OnLoginCompleteDelegates->Clear();
	}
	OSSIdentity->OnLoginCompleteDelegates->AddLambda([this] (int32 LocalUserNum, bool bSuccess, const FUniqueNetId& NewUNI, const FString& Error)
	{
		UE_LOG
			( LogNet
			, Display
			, TEXT("%s: Login of player num %d: %s")
			, *GetFullName()
			, LocalUserNum
			, bSuccess ? TEXT("success") : TEXT("failure")
			)

		UMyLocalPlayer* LocalPlayer = Cast<UMyLocalPlayer>(GetGameInstance()->GetLocalPlayerByIndex(LocalUserNum));
		AHUD_MainMenu* HUDMenu = LocalPlayer->GetPlayerController(GetWorld())->GetHUD<AHUD_MainMenu>();
		
		if(bSuccess)
		{
			LocalPlayer->IsLoggedIn = true;
			LocalPlayer->SetCachedUniqueNetId(FUniqueNetIdRepl(NewUNI));
			
			Cast<UMyGameInstance>(GetGameInstance())->SessionConfig.bEnableLAN = false;
			// TODO: visualize successful login
		}
	});
	
	// logout handler
	
	if(OSSIdentity->OnLogoutCompleteDelegates->IsBound())
	{
		UE_LOG(LogNet, Warning, TEXT("%s: OnLogoutCompleteDelegates: was bound, clearing"), *GetFullName())
		OSSIdentity->OnLogoutCompleteDelegates->Clear();
	}
	OSSIdentity->OnLogoutCompleteDelegates->AddLambda([this] (int32 PlayerNum, bool bSuccess)
	{
		if(bSuccess)
		{
			Cast<UMyLocalPlayer>(GetGameInstance()->GetLocalPlayerByIndex(PlayerNum))->IsLoggedIn = false;
		}
		UE_LOG
			( LogNet
			, Display
			, TEXT("%s: Player %d log out: %s")
			, *GetFullName()
			, PlayerNum
			, bSuccess ? TEXT("success") : TEXT("failure")
			)
	});
}

IOnlineSessionPtr UMyGISubsystem::GetSessionInterface() const
{
	return Online::GetSessionInterfaceChecked
		( GetWorld()
		, Cast<UMyGameInstance>(GetGameInstance())->SessionConfig.bEnableLAN
			? FName(TEXT("NULL"))
			: FName(TEXT("EOS"))
		);
}
