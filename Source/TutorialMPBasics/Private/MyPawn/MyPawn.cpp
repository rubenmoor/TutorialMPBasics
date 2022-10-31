// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn/MyPawn.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Body")));
	SetRootComponent(Body);

    AActor::SetReplicateMovement(false);
}

void AMyPawn::AccelerateLeft()
{
	Velocity += FVector(0, -10, 0);
}

void AMyPawn::AccelerateRight()
{
	Velocity += FVector(0, 10, 0);
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
}

void AMyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPawn, Velocity);
}