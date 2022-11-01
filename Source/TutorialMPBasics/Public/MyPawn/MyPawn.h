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
	TObjectPtr<class USphereComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Body;
	
public:
	// `Velocity` is a replicated property. This means: a change of a pawn's velocity propagates from the host to
	// the clients. It doesn't matter who controls the pawn. For the information flow in the other direction, see
	// "Modes/PlayerController.cpp".
	// Note that movement replication is turned off. With the velocity replicated, the pawn has all the information
	// required to correctly move in-sync.
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity = FVector::Zero();

	void AccelerateLeft();
	void AccelerateRight();

	// event handlers
	virtual void Tick(float DeltaTime) override;
};
