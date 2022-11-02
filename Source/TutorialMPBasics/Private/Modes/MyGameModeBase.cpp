// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyGameModeBase.h"

#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/MyPlayerController.h"
#include "MyPawn/MyPawn.h"

#define LOCTEXT_NAMESPACE "GameMode"

void AMyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(!IsValid(NewPlayer->GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No pawn spawned."), *GetFullName())
		// Unfortunately, `KickPlayer` causes a network failure with the client and I don't know how to
		// respond to that properly. Therefore, we tell the client to quit themselves
		//GameSession->KickPlayer(NewPlayer, LOCTEXT("CouldntSpawn", "Could not spawn pawn"));
		Cast<AMyPlayerController>(NewPlayer)->ClientRPC_LeaveSession();
	}
}

AActor* AMyGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	// Don't call super, we implement our own independent method
	//return Super::ChoosePlayerStart_Implementation(Player);
	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Starts);
	// simply fill the PlayerStarts in order
	return Starts[GetNumPlayers() - 1];
}

#undef LOCTEXT_NAMESPACE
