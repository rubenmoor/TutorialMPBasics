// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UCLASS()
class TUTORIALMPBASICS_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// VisibleAnywhere on properties that are pointers to UObjects (like this one) does allow editing of its properties;
	// If you put "EditAnywhere" instead, you could change the type of Body (e.g. from UStaticMeshComponent to
	// USplineComponent), which isn't what you usually want
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Body;
	
public:	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity = FVector::Zero();

	void AccelerateLeft();
	void AccelerateRight();

	// event handlers
	virtual void Tick(float DeltaTime) override;
};
