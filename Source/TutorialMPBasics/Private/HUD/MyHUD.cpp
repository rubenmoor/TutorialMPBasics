// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/UW_HUD.h"

#define LOCTEXT_NAMESPACE "HUD"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	UW_HUD = CreateWidget<UUW_HUD>(GetGameInstance(), UW_HUD_Class, FName(TEXT("HUD")));
	UW_HUD->AddToViewport();
	
	UW_HUD->SetTextHostClient(LOCTEXT("host-client", "helo"));
}

#undef LOCTEXT_NAMESPACE