// Fill out your copyright notice in the Description page of Project Settings.


#include "Modes/MyGameModeBase.h"

AActor* AMyGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}
