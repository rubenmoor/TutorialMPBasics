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

	// using a smart pointer, implying proper clean up of the object created with `new`
	const TSharedRef<FOnlineSessionSettings> LastSessionSettings = MakeShared<FOnlineSessionSettings>();
	// similar, but allows several pointers to point to the created object:
	//const TSharedPtr<FOnlineSessionSettings> LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	// cf. https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/SmartPointerLibrary/
	
	// using the `bPrivate` variable to either set the number of **private** or **public** connections
	// respectively
	LastSessionSettings->NumPrivateConnections = bPrivate ? NumConnections : 0;
	LastSessionSettings->NumPublicConnections = !bPrivate ? NumConnections : 0;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	LastSessionSettings->bIsDedicated = false;
	// `bUsesPresence` means that this session will affect the status displayed in the online service (Steam, EOS)
	// At least this is how I understand it. Cf. https://forums.unrealengine.com/t/what-is-a-presence-session/327223/2
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = bEnableLAN;
	LastSessionSettings->bShouldAdvertise = true;

	LastSessionSettings->Set(SETTING_MAPNAME, FString(TEXT("some level")), EOnlineDataAdvertisementType::ViaOnlineService);

	// the custom name for the session, provided by the user
	LastSessionSettings->Set(SETTING_CUSTOMNAME, CustomName, EOnlineDataAdvertisementType::ViaOnlineService);
	// FOnlineSessionSettings::Set allows to set [value] at [key], where value must be one of
	// TSharedRef, uint64, int64, TArray<uint8>, float, double, bool, uint32, int32, TCHAR*, FString
	// Our enum isn't one of those, luckily in can be cast to `int32` trivially;
	// its internal representation is `uint8`, which converts to `int32` w/o problem
	LastSessionSettings->Set(SETTING_LEVEL, (int32)ECurrentLevel::SomeLevel);

	// As we are dealing with a multicast delegate, we can add as many delegates (= handlers) as we want.
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

	 * ... where `HandleCreateSessionComplete` is your custom function that implements the same functionality as is
	 * passed inside `Callback`.
	 * In order to see what `Callback` actually does, e.g. what code gets executed, see the call to `CreateSession` in
	 * "UMyGameInstance.cpp", lines 15-39
	 * 
	 */
	SI->OnCreateSessionCompleteDelegates.AddLambda(Callback);

	/*
	 * Maybe you have seen code like this in some tutorial:

	return SI->CreateSession(*LPC.GetLocalPlayer()->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);

	 * However, this signature passes along a unique net id, which might be `null` at this point.
	 * By passing the local player index, a unique net id will be created for us without problem.
	 * Note that often, instead of `GetLocalPlayer()->GetIndexInGameInstance()` just `0` is provided.
	 * The 0 means that the session is always created in the name of the local player with index 0.
	 * This is always safe, as split/shared screen isn't usually possible at the same time as LAN/online multiplayer.
	 * As long as there is only one local player (not split/shared screen), its index is 0.
	 * 
	 * Also, the parameter `FName SessionName`, set to `NAME_GameSession`, can be any `FName`,
	 * e.g. `FName(TEXT("my custom session")`. However, don't mistake the `SessionName` parameter for some sort of
	 * session description. Unreals FNames are	case-insensitive identifiers that aren't meant to hold
	 * user content (or any dynamic content for that matter).
	 * `NAME_GameSession` is sort of a default value for the kind of session we are creating (i.e. opposed to
	 * `NAME_PartySession` which is meant to be used for lobby sessions before starting the game).
	 * In the end, you can put any FName there. Just make sure to be consistent in always putting the same.
	 * 
	 */
	return SI->CreateSession(LPC.GetLocalPlayer()->GetIndexInGameInstance(), NAME_GameSession, *LastSessionSettings);
}

void UMyGISubsystem::JoinSession(const FLocalPlayerContext& LPC, TFunction<void(ECurrentLevel, EOnJoinSessionCompleteResult::Type)> Callback)
{
	const IOnlineSessionPtr SI = GetSessionInterface();
	const TSharedRef<FOnlineSessionSearch> LastSessionSearch = MakeShared<FOnlineSessionSearch>();
	LastSessionSearch->MaxSearchResults = 10000;
	LastSessionSearch->bIsLanQuery = Cast<UMyGameInstance>(GetGameInstance())->SessionConfig.bEnableLAN;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	if(SI->OnFindSessionsCompleteDelegates.IsBound())
	{
		UE_LOG(LogNet, Warning, TEXT("%s: OnFindSessionsCompleteDelegates: was bound, clearing"), *GetFullName())
		SI->OnFindSessionsCompleteDelegates.Clear();
	}
	SI->OnFindSessionsCompleteDelegates.AddLambda([this, LastSessionSearch, LPC, Callback, SI] (bool bSuccess)
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
			// ... we lookup the new level in the session settings ...
			// ... and when we joined a session ...
			// ... execute `Callback(NewLevel, EJoinSessionCompleteResult::Type)`
			int32 NewLevelI;
			// the session settings can't store our enum `CurrentLevel`, we stored an `int32` instead
			Results[0].Session.SessionSettings.Get(SETTING_LEVEL, NewLevelI);
			SI->OnJoinSessionCompleteDelegates.AddLambda([Callback, NewLevelI] (FName, EOnJoinSessionCompleteResult::Type Type)
			{
				// to convert `int32` to the enum, `static_cast` is just fine
				Callback(static_cast<ECurrentLevel>(NewLevelI), Type);
			});
			SI->JoinSession(LPC.GetLocalPlayer()->GetIndexInGameInstance(), NAME_GameSession, Results[0]);
		}
		else
		{
			UE_LOG(LogNet, Error, TEXT("%s: couldn't find session."), *GetFullName())
		}
	});

	// after having registered the callback (`AddLambda`) for the FindSessionCompleteEvent, we go and find sessions
	SI->FindSessions
		(LPC.GetLocalPlayer()->GetIndexInGameInstance()
		, LastSessionSearch
		);
}

void UMyGISubsystem::LeaveSession()
{
	const IOnlineSessionPtr SI = GetSessionInterface();
	if(SI->GetNamedSession(NAME_GameSession))
	{
		if(SI->OnDestroySessionCompleteDelegates.IsBound())
		{
			UE_LOG(LogNet, Warning, TEXT("%s: OnDestroySessionCompleteDelegates: was bound, clearing"), *GetFullName())
			SI->OnDestroySessionCompleteDelegates.Clear();
		}
		SI->OnDestroySessionCompleteDelegates.AddLambda([this, SI] (FName, bool bSuccess)
		{
			// `DestroySession` does seem to have some glitches, where the session ends up not being destroyed.
			// Unfortunately, I regularly encounter the case where `bSuccess` is true, but the session isn't destroyed.
			if(SI->GetNamedSession(NAME_GameSession))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s: Failed to destroy session, trying again ..."), *GetFullName())
				SI->DestroySession(NAME_GameSession);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("%s: Session destroyed."), *GetFullName())
				GetGameInstance()->ReturnToMainMenu();
			}
		});
		SI->DestroySession(NAME_GameSession);
	}
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

	// When EOS not configured, warn right away.

	/*
	 * `GetIdentityInterfaceChecked(FName(TEXT("EOS"))` causes a crash when EOS isn't configured
	 */
	const IOnlineIdentityPtr OSSIdentity = Online::GetIdentityInterface(GetWorld(), FName(TEXT("EOS")));
	if(!OSSIdentity.IsValid())
	{
		UE_LOG
			( LogNet
			, Warning
			, TEXT("%s: couldn't get identity interface for EOS. Probably not configured.")
			, *GetFullName()
			)
		return;
	}
	
	// handle network failure
	// this isn't working for some reason, cf. https://forums.unrealengine.com/t/delegate-for-handlenetworkfailure-doesnt-fire/681949/4
	// The delegate never fires, even if I get "Network Failure" in my log.
	GEngine->OnNetworkFailure().AddLambda([this] (UWorld*, UNetDriver*, ENetworkFailure::Type, const FString& StrError)
	{
		UE_LOG(LogNet, Error, TEXT("%s: NetworkFailure: %s"), *GetFullName(), *StrError)
		LeaveSession();
	});

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
