// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HUD.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class TUTORIALMPBASICS_API UUW_HUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTextHostClient(FText InText) const;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextHostClient;
};
