// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UW_HUD.h"

#include "Components/TextBlock.h"

void UUW_HUD::SetTextHostClient(FText InText) const
{
	TextHostClient->SetText(InText);
}
