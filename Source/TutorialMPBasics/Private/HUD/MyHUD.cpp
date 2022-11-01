// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/UW_HUD.h"

#define LOCTEXT_NAMESPACE "HUD"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	if(!IsValid(UW_HUD_Class))
	{
		UE_LOG(LogTemp, Error, TEXT("%s: UW_HUD_Class null"), *GetFullName())
		return;
	}
	UW_HUD = CreateWidget<UUW_HUD>(GetGameInstance(), UW_HUD_Class, FName(TEXT("HUD")));
	UW_HUD->AddToViewport();

	// `AHUD::GetLocalRole()` is always `ROLE_Authority` as the local player apparently has authority over their HUD,
	// whereas the `GetOwningPlayerController()` allows us to distinguish between host and client.
	const FText HostClient = GetOwningPlayerController()->GetLocalRole() == ROLE_Authority
		? LOCTEXT("host", "host")
		: LOCTEXT("client", "client");
	UW_HUD->SetTextHostClient(HostClient);
}

#undef LOCTEXT_NAMESPACE