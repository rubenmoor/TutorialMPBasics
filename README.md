WIP

# Introduction

This is a newby-friendly tutorial for the use of Unreal Engine with C++ (as opposed to BluePrint).
It covers a basic file setup that is useful for really any type of game, but especially if you want to implement any sort of multiplayer.

## What this tutorial is about

Unreal Engine documentation is often incomplete.
With regards to multiplayer, this was/is especially annoying.
There is information on the power of [Actor Replication](https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Networking/Actors/),
great, and I can learn about the role of different classes of [Unreal's Gameplay Framework](https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Framework/),
but try to get multiplayer going for a fresh C++ project and you are up for a rough start.

The setup that I am going to present here will have you prepared for any of the following scenarios:

* Singleplayer
* Split or shared screen
* LAN multiplayer
* Online multiplayer (e.g. via Epic Online Services or Steam)

The Generic LAN interface is important for me, as it is supposed to be an easy setup.
Also, I like the idea of offering multiplayer without any account registration required.
This is controversial though, as it plays into the hands of software piracy, too.

For online multiplayer to work, I rely on the [EOS Online subsystem plugin](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/EOS/),
i.e. the online subsystem for the [Epic Online Services](https://dev.epicgames.com/en-US/home).

TODO: Steam (should be relatively straightforward)

## Specifically, this tutorial will have answers for the following questions

(I might be wrong, but to me these questions are essential and online search results left me in the dark)

* What is the unique net id? Do I have to bother? Should I store it?
* How to have more than one local player and should I bother?
* What is `ULocalPlayer` good for and how does it relate to `GameInstance` and the usual suspects `PlayerState` and `PlayerController`?

And this [Local Player Context](https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Engine/FLocalPlayerContext/) looks useful.
Let's actually go ahead and use it.

# Part 1: Using Game Instance to manage application state

The Game Instance is an object of type `UGameInstance`, or of a type that inherit from `UGameInstance`.
The recommended workflow is a customization of your Game Instance according these steps:

1. Create a new C++ class from within the editor
   * select `UGameInstance` as parent class
   * call it "MyGameInstance"
   * Note that Unreal automatically adds a "U", thus resulting in `UMyGameInstance`
2. Recompile your code, depending on your live coding settings, an editor restart is required
3. Create a new BluePrint that inherits from your new `MyGameInstance` and call it "BP_MyGameInstance"
4. In the editor, in your project settings, in *Project* -> *Maps and Modes* all the way at the bottom select as custom Game Instance Class `BP_MyGameInstance`

These steps are the standard way of amending all kinds of classes of the Gameplay Framework, like `APlayerController`, `AGameMode` or `AGameModeBase`, `AGameState` or `AGameStateBase`, `APlayerState`, `AHUD`.
Luckily, you can work with the "vanilla versions" all the way until you actually need to add properties or methods.
It is possible to work with `BP_HUD`, deriving from `AHUD` and then, one day, rewire the `BP_HUD` to inherit from some custom `AMyHUD`, which in turn inherits from `AHUD`.

## Using `UMyGameInstance` in your code

Any property you add to your class `UMyGameInstance` you can decorate with the `UPROPERTY` macro and it will show up in the BluePrint `BP_GameInstance` with the value you set in C++ as a default.
You will see examples for the use of `UPROPERTY` below. 
You can then set any property value in the BluePrint without the need to recompile.

Within your C++ code you get your custom Game Instance object like this:

`UMyGameInstance* GI = GetGameInstance<UMyGameInstance>();`

or when the templated version isn't available you can use the equivalent code

`UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());`.

In both cases, `GI` will only be `null` when you didn't configure the correct class in step 4 above. 
`Cast` is a C++ function from Unreal that uses Unreals `UCLASS` system to dynamically cast `UObject`s.
Only if you don't deal with `UObject`s (that is: almost never), you have to explicitly use [C++-style casts](https://stackoverflow.com/questions/28002/regular-cast-vs-static-cast-vs-dynamic-cast) like `static_cast` or `dynamic_cast`.

## Game Instance gets initialized once and stays with us for the whole time the game is running

The game instance object gets initialized with the start of your game.
It's available as a global object from anywhere in your code.
Be aware however, that the game instance is local and is unaware of anything related to networking or multiplayer.
When the user quits your game, the game instance gets destroyed.

Another defining aspect of the game instance object:
It's the same object during its whole time of existence.
Other objects from Unreal's Gameplay Framework are allowed to change (e.g. the Player Controller, Player State, Game State, Game Mode, HUD).
For example, you can create maps "Level1" and "Level2" that set the Player Controller class to `BP_PlayerContoller1` and `BP_PlayerController2`, respectively. `GetPlayerController<UPlayerController1>` while Level 2 is loaded will then return `null`.

Be aware of one common beginner's mistake:
From within the constructors of an actor or a component, you cannot get the Game Instance.
`GetGameInstance()` within constructors return `null`.
Same for the `OnConstruction` method.
Both, the constructor and `OnConstruction` get called not only when your game initially starts but also when the editor simply loads your game
(or when you create an actor within in the editor).
The constructor isn't aware of the Gameplay Framework and doesn't count on anything being loaded yet, not even the Game Instance.

## Application state mostly concerns the User Interface

In case you have had the pleasure of programming a user interface, e.g. a website, and you used some framework, you might know what "application state" means.
If you can restore the state an application was in by reading a couple of variables, those variables are the application state.
In Unreal, application state doesn't include anything that happens within a level.
For this, there is the Player State (and the Game Mode and Game State).
The name of the currently loaded level, if any, is part of the application state again.

Before we actually implement variables of the application state, we have to introduce a different class.

## Taking into account local players saves from headaches later on

In Unreal's `UGameInstance` in the file "GameInstance.h" you can find these lines:

```
/** List of locally participating players in this game instance */
	UPROPERTY()
	TArray<TObjectPtr<ULocalPlayer>> LocalPlayers;
```

This is an array of local players, which will quite often only consist of one entry.
You only ever have to deal with more than one local player in the following scenarios:

* Split screen multiplayer
* Shared screen multiplayer
* Testing networked multiplayer using the "Run under one process" option

If you are positive that you won't encounter any of these scenarios, you can ignore `ULocalPlayer` like most people do.
When you see this in someone's code, `GetPlayerController(0)`, or similarly `GetPrimaryPlayerController()`,
very likely someone just assumed that there is only ever one player: The one with index 0, or the "primary" local player.

It doesn't cost anythying, however, to account for potentially multiple local players.
This is why I recommend to create your custom local player type "MyLocalPlayer" that inherits from `ULocalPlayer`.
You then set the local player class in the project settings under *Engine - General Settings* -> *Default Classes*.
(It isn't possible to create a BluePrint that inherits from `LocalPlayer` or `MyLocalPlayer`, probably the local player isn't a `UObject`)

Now we have everything together to implement the application state.
By putting application state into the `MyLocalPlayer` class, it is technically still within the game instance, only one step deeper.
Remember that the game instance holds an array of all `MyLocalPlayer` objects.
... with the added benefit that two different local players can, in principle, have the same game instance with individual state.

## Implementing application state

To this end, I find the following `enum` type useful:

File: [Modes/MyLocalPlayer.h](/Source/TutorialMPBasics/Public/Modes/MyLocalPlayer.h)

```
// MyLocalPlayer.h

UENUM(BlueprintType)
enum class ECurrentLevel : uint8
{
	MainMenu UMETA(DisplayName="main menu"),
	Level0 UMETA(DisplayName="level 0"),
  Level1 UMETA(DisplayName="level 1")
  // ...
};

UCLASS()
class  MYGAME_API UMyLocalPlayer : public ULocalPlayer
{
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECurrentLevel CurrentLevel;
// ...
```

along with the helper method

```
UFUNCTION(BlueprintCallable)
bool GetIsInMainMenu()
{
  return CurrentLevel == ECurrentLevel::MainMenu;
}
```
.

Now we can call `GetLocalPlayer().CurrentLevel = ECurrentLevel::Level1` or `GetLocalPlayer().GetIsInMainMenu()`,
e.g. from within a Player Controller object.
Or, in case we don't care of other local players then the first one, we can call e.g. from within the Game Instance `GetLocalPlayerByIndex(0).CurrentLevel`.

## Session configuration for multiplayer

As mentioned earlier, application state **does** include the state of the user interface.
In order to get a minimal, funtional multiplayer setup, there will be a menu in the game to configure a session in order to host a game.
This session configuration holds the following information, give or take:

* A custom server name
* Whether the server is private or public (e.g. by means of a checkbox)
* Whether the server is LAN-only or online
* How many players are allowed to join
* The game mode/map of the hosted game

I had difficulties imagining a split screen scenario where two local players configure a session independently.
Also, for the purposes of testing multiplayer with the "Run under one process" option enabled, there is one host and everyone else joins there.
Thus, I feel comfortable to implement the session configuration in `MyGameInstance.h`:

File: [Modes/MyGameInstance.h](/Source/TutorialMPBasics/Public/Modes/MyGameInstance.h)

```
// MyGameInstance.h

UENUM(BlueprintType)
enum class EGameMode : uint8
{
        // examples for game modes (i.e. rule sets) of your game
	EveryManForHimself UMETA(DisplayName="every man for himself"),
	Teams UMETA(DisplayName="teams"),
	Coop UMETA(DisplayName="coop"),
};

/*
 * minimal session setup information
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
class MYGAME_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FHostSessionConfig SessionConfig = // default values
		{ ""
		, 4
		, false
		, true
		, EGameMode::EveryManForHimself
		};
};
```

Later, when the player interacts with the user interface, enabling the corresponding checkbox will set `GetGameInstance<UMyGameInstance>()->SessionConfig.bPrivate = true`.
And then, when the player clicks a button called "Host", we will call a function to start up a session that reads out the values of `SessionConfig`.

# Part 2: Managing sessions

Regarding sessions, you can find incomplete information in different places.

* [UE 4.27 Official Documentation of the Session interface](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/SessionInterface/) The article still applies for UE 5, it gives a general overview on the topic and lacks details and depth
* [A wiki article](https://unreal.gg-labs.com/wiki-archives/networking/how-to-use-sessions-in-c++) It seems outdated, the code still works
* [Another wiki article](https://unrealcommunity.wiki/online-multiplayer-vkje2zyn) The information in those two articles has considerable overlap
* [A famous blog arcticle](https://cedric-neukirchen.net/2021/06/27/ue4-multiplayer-sessions-in-c/) There is a [Github Repo with all the relevant code](https://github.com/eXifreXi/CppSessions) linked
* [A Youtube-tutorial for UE5](https://www.youtube.com/watch?v=jc0COamYRm4)
* [The source code of the Advanved Session Plugin on GitHub](https://github.com/mordentral/AdvancedSessionsPlugin)

I said "incomplete" because none of the above source presented me a complete solution.
These source are of course voluntary efforts of the community, so there is no reason to complain.
But a comprehensive documentation that explains what's going on and offers ways of understanding is still missing.
Let's start and fill the gap.

## A session has lifecycle, from being created to being destroyed, and other players can join

When talking about sessions, we are leaving split/shared screen behind.
Usually, in the session tutorials, it is assumed that there is only one local player (index 0).
The Unreal Editor, however, offers a "run under one process" option to test multiplayer from within the editor (Play in Editor = PIE).
In this, admittedly quite specific, testing scenario, there are several local players joining a LAN session.

The session is part of the application state.
Both, properties and methods, can be put into our custom Game Instance `UMyGameInstance`.
However, it seems to be good practice to use what is called the "Game Instance Subsystem" for session management.
"Subsystem" here is just a fancy word.
The Game Instance Subsystem is simply another object that shares all the sweet properties of Game Instance.
So this is all about giving some structure to your code, not more.

You can read more about the idea behind subsystems Ben's [Unreal-syle Singletons](https://benui.ca/unreal/subsystem-singleton/).

By creating a new C++ class (from within the Unreal Editor) that inherits from `GameInstanceSubsystem`, you are good to go.
I called mine `MyGISubsystem`, but `SessionManager` would be an equally valid name choice.

You can always find the complete code in the corresponding source files.
I.e. [Modes/MyGISubsystem.h](/Source/TutorialMPBasics/Public/Modes/MyGISubsystem.h) and [Modes/MyGISubsystem.cpp](/Source/TutorialMPBasics/Private/Modes/MyGISubsystem.cpp).
In this document, I present the code in fractions.

### Chosing the online subsytem: LAN, EOS, Steam, or whatnot.

The online subsystem shouldn't be confused with the above Game Instance Subsystem. 
I don't know why they are both called subsystem, they don't have anything in common.
"Online Subystem" always refers to Steam or EOS (the Epic Online Services) and alike.
You can set a default for the online subsystem by adding the following line to [Config/DefaultEngine.ini](/Config/DefaultEngine.ini)

```
[OnlineSubsystem]
DefaultPlatformService=<Default Platform Identifier>
```

as documented in [the official docs on the online subsystem](https://docs.unrealengine.com/5.0/en-US/online-subsystem-in-unreal-engine/).
The `<Default Platform Identifier>` can be `NULL`, `Steam`, `EOS`, `EOSPlus`, depending on your choice.
I prefer not to set the default and rather make an explicit choice of the online subsytem.
The reason for that is that I want to implement LAN (no authentication required!) along with some online service.
If you let your player chose between single player and, say, Steam, it makes sense to set Steam as default.

In our case, we define the following function inside the Game Instance Subsystem to get the right *online* subsystem:

File [Modes/MyGISubsystem.cpp](/Source/TutorialMPBasics/Private/Modes/MyGISubsystem.cpp)

```
IOnlineSessionPtr UMyGISubsystem::GetSessionInterface() const
{
	return Online::GetSessionInterfaceChecked
		( GetWorld()
		, Cast<UMyGameInstance>(GetGameInstance())->SessionConfig.bEnableLAN
			? FName(TEXT("NULL"))
			: FName(TEXT("EOS"))
		);
}
```

This code checks the application state to see whether or not we have LAN enabled to chose between LAN (encoded as the "Online Subystem NULL") and the Epic Online Services/EOS.
The idea is to implement a UI that has a checkbox to set the `bEnableLAN` value in the game instance.

### Creating a session

Just follow the comments inside the code to get what's going on.

File [Modes/MyGISubsystem.cpp](/Source/TutorialMPBasics/Private/Modes/MyGISubsystem.cpp)

```
bool UMyGISubsystem::CreateSession(const FLocalPlayerContext& LPC, FHostSessionConfig SessionConfig,
                                   TFunction<void(FName, bool)> Callback)
{
	// the aforementioned helper function to get the right online subsystem based on the application state
	const IOnlineSessionPtr SI = GetSessionInterface();

	// syntax for unpacking of structs
	auto [ CustomName, NumConnections, bPrivate, bEnableLAN, _GameMode ] = SessionConfig;
	
	// using a smart pointer, implying proper clean up of the object created with `new`
	const TUniquePtr<FOnlineSessionSettings> LastSessionSettings = MakeUnique<FOnlineSessionSettings>();
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

	// The custom name for the session, provided by the user.
	// This requires `#define SETTING_CUSTOMNAME FName(TEXT("CUSTOMNAME"))` in `MyGISubsystem.h`, right after the includes.
	LastSessionSettings->Set(SETTING_CUSTOMNAME, CustomName, EOnlineDataAdvertisementType::ViaOnlineService);

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
	 */
	return SI->CreateSession(LPC.GetLocalPlayer()->GetIndexInGameInstance(), NAME_GameSession, *LastSessionSettings);
}
```

Given that `UMyGISubsystem::CreateSession` has the `TFunction` object "Callback" as parameter,
and given that `Callback` gets registered as delegate handler via `OnCreateSessionCompleteDelegates.AddLambda(Callback)`,
we get to decide what happens at the event "OnCreateSessionComplete" right where we call `UMyGISubsystem::CreateSession`.

For that, we define `UMyGameInstance::HostGame`, like this:

File: [Modes/UMyGameInstance.cpp](Source/TutorialMPBasics/Private/Modes/MyGameInstance.cpp)

```
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
```

This method in the Game Instance doesn't have a lot to do.
The idea is that some button in the user interface of the same name ("Host Game") is bound to the `HostGame` method of the Game Instance.

The guiding principle is:
The user interface (UI) is implemented as a HUD (inheriting from `AHUD`) in a level called "MainMenu".
All buttons of the UI (or call it the Main Menu, or the HUD) either

* bind to functions from within the UI, e.g. when you navigate through menus,
* or they set some application state, e.g. a checkbox to enable LAN
* or they call a method of the Game Instance, like `HostGame`

Thus, effectively the UI is only concerned with itself and the Game Instance.
Also, the UI is owned by a local player (and thus by a player controller).
I guess this kind of makes sense, as the navigation through the menu usually is the responsibility of one player, even in a shared screen scenario.
If you want local players to configure stuff individually, e.g. the haircut of their respective characters, you are in a split-screen scenario with two distinct UIs.
As the owning player of the UI comes as a package consisting of

* a `ULocalPlayer` object
* a `APlayerController` object
* a `APlayerState` object

there is the `FLocalPlayerContext`, cf. [official documentation](https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Engine/FLocalPlayerContext/).
The methods of the Game Instance that get called from the UI all take a `const FLocalPlayerContext& LPC` as an argument.
This way the Game Instance is always aware of the actual local player (and its controller) when applying changes to the application state.

### The local player context is somewhat incompatible to Unreal Engine

The developers of Unreal Engine leave us with a somewhat stupid choice, regarding the use of the local player context.
I would like to define `HostGame` like this:

```
// this doesn't compile
UFUNCTION(BlueprintCallable)
void HostGame(const FLocalPlayerContext& LPC);
```

This way, I would have the choice to implement UI related stuff completely in Blueprint.
The Game Instance is the perfect interace from Blueprint to C++.
Furthermore you can trust me that `const FLocalPlayerContext&` type is the preferred way of passing structs as function parameters in C++.
However, the definition of `FLocalPlayerContext` lacks the `USTRUCT` macro (unlike our custom `FSessionConfig`), which makes it incompatible with the `UFUNCTION` macro.
I really like the idea behind the `FLocalPlayerContext` for the convenience (and clarity) it provides.
It seems that its use isn't really encouraged though.
If you want to be more Blueprint-compatible, you can replace the `FLocalPlayerContext` param with the index of the local player in the array of the Game Instance, `int32`.
Via `AGameInstance::GetLocalPlayerByIndex(const int32) you can get access to the local player, and from there to the Player Controller, **and from there** to the Player State, in case you need it.
From within the UI, you will need `ULocalPlayer::GetIndexInGameInstace() -> int32` to get the local player index.
