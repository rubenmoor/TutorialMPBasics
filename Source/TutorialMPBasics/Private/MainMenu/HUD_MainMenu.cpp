// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/HUD_MainMenu.h"

#include "MainMenu/UW_MainMenu.h"
#include "Blueprint/UserWidget.h"

void AHUD_MainMenu::BeginPlay()
{
	// this method can be equally well implemented in Blueprint without problem
	// I just happen to prefer C++ code in about 100% of the cases
	Super::BeginPlay();

	if(!IsValid(MainMenuClass))
	{
		// in case we forgot to set the MainMenuClass field in the Blueprint "BP_HUD_MainMenu", we want an error
		// that reminds us instead of a crash of the editor
		UE_LOG
			// `LogActor` isn't exactly a meaningful category for logging, but neither "LogUI" nor "LogHUD" exist
			// and LogSlate implies an additional include
			( LogActor 
			, Error
			, TEXT("%s: MainMenuClass null")
			// always including `GetFullName()` like this, helps us identifying the exact object that caused this
			// custom error (or other log output), at the expense of more verbose output
			, *GetFullName()
			)
		return;
	}

	// without `#include "MainMenu/UW_MainMenu.h"` this doesn't compile; apparently the compiler has to be made aware
	// of the fact that `UUW_MainMenu` inherits from `UUserWidget`
	MainMenu = CreateWidget<UUW_MainMenu>(GetGameInstance(), MainMenuClass, FName(TEXT("Main Menu")));
	MainMenu->AddToViewport();
}
