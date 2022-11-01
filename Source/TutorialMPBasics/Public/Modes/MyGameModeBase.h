// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// event handlers
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// The Unreal default mechanism for choosing a player start seems broken
	// cf. https://forums.unrealengine.com/t/playerstart-actors-for-multiplayer-disfunct-or-what-am-i-doing-wrong/681970
	// (In principle, it allows to spawn actors in a randomly chosen player start, where a collision is
	// handled depending on the "Spawn collision handling method" of the PlayerStart actor)
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
