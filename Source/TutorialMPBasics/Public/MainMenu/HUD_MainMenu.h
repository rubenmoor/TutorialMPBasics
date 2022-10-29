// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUD_MainMenu.generated.h"

class UUW_MainMenu;
/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API AHUD_MainMenu : public AHUD
{
	GENERATED_BODY()

	
protected:
	// This is an editable property, we have to set "MainMenuClass" to `BP_UW_MainMenu` inside the editor.
	// Note that from within C++, we don't have access to Blueprints
	// (unless you are fine with using hard coded asset paths via `FObjectFinder`)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUW_MainMenu> MainMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UUW_MainMenu> MainMenu;
	
	virtual void BeginPlay() override;
};
