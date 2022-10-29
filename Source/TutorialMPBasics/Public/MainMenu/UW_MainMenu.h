// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API UUW_MainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	// event handlers
	virtual void NativeConstruct() override;
	
	// `meta=(BindWidget)` implies that a Blueprint that inherits from UUW_MainMenu has somewhere
	// in its widget hierarchy a `UButton` named BtnHost
	// the `BindWidget` feature allows to take control over that Button in the Blueprint from inside the C++ code
	UPROPERTY(meta=(BindWidget))
	// `class UButton` informs the compiler that some class called `UButton` exists, without declaring or including it
	// alternatively, one can add `#include "Components/Button.h"` above;
	// or, one can add `class UButton;` below the include list
	// reducing the number of includes in .h files generally reduces compiles times;
	// note, however, in .cpp files the full include is usually necessary
	TObjectPtr<class UButton> BtnHost;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnJoin;

	// delegate handlers
	UFUNCTION()
	void BtnHostClicked();

	UFUNCTION()
	void BtnJoinClicked();
};
