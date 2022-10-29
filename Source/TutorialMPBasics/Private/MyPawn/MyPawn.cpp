// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn/MyPawn.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Body")));
	SetRootComponent(Body);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
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
	const float Dampening = 10.;
	Velocity = Velocity.Length() < Dampening * DeltaTime ? FVector::Zero() : Velocity + Velocity.GetUnsafeNormal() * -Dampening * DeltaTime;
}

