// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/UW_MainMenu.h"

#include "Components/Button.h"
#include "Modes/MyGameInstance.h"

void UUW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// unfortunately, dynamic delegates do not support closures, thus we implement the function-pointer
	BtnHost->OnClicked.AddDynamic(this, &UUW_MainMenu::BtnHostClicked);
	BtnJoin->OnClicked.AddDynamic(this, &UUW_MainMenu::BtnJoinClicked);
}

void UUW_MainMenu::BtnHostClicked()
{
	GetGameInstance<UMyGameInstance>()->HostGame(GetPlayerContext());
}

void UUW_MainMenu::BtnJoinClicked()
{
	GetGameInstance<UMyGameInstance>()->JoinGame(GetPlayerContext());
}
