// Fill out your copyright notice in the Description page of Project Settings.


#include "Coffin.h"

// Sets default values
ACoffin::ACoffin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACoffin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoffin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

