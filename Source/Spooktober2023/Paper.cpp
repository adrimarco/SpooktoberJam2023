// Fill out your copyright notice in the Description page of Project Settings.


#include "Paper.h"

// Sets default values
APaper::APaper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("Interactable");

	// Paper mesh
	paperMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Paper Mesh"));
	SetRootComponent(paperMesh);
}


// Called when the game starts or when spawned
void APaper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APaper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APaper::SetPaperMessage(FText text) {
	message = text;
}

void APaper::SetPaperTitle(FName t) {
	title = t;
}

FText const APaper::GetPaperMessage() {
	return message;
}

FName const APaper::GetPaperTitle() {
	return title;
}
