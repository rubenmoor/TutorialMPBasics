// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API AMyHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUW_HUD> UW_HUD_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UUW_HUD> UW_HUD;
};
