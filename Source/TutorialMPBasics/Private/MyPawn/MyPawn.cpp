// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn/MyPawn.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USphereComponent>(FName(TEXT("SphereCollision")));
	SetRootComponent(Root);
	Root->SetSphereRadius(50);
	
	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Body")));
	Body->SetupAttachment(Root);

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

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// move the pawn according to its velocity
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
}

void AMyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPawn, Velocity);
}