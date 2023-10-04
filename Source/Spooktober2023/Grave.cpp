// Fill out your copyright notice in the Description page of Project Settings.


#include "Grave.h"
#include "GraveGround.h"

// Sets default values
AGrave::AGrave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	graveGroundActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Grave Ground Actor"));
}

// Called when the game starts or when spawned
void AGrave::BeginPlay()
{
	Super::BeginPlay();
	
	if(groundBP != nullptr)
		graveGroundActor->SetChildActorClass(groundBP);
	else {
		graveGroundActor->Deactivate();
		graveGroundActor = nullptr;
	}
}

// Called every frame
void AGrave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

