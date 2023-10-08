// Fill out your copyright notice in the Description page of Project Settings.


#include "Coffin.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.h"

constexpr auto COFFIN_OPENED_ROTATION = 70.f;

// Sets default values
ACoffin::ACoffin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	auto root = GetRootComponent();

	jewels.Empty();

	// Mesh components
	coffinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coffin Mesh"));
	coffinMesh->SetupAttachment(root);

	coffinDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Coffin Door Mesh"));
	coffinDoorMesh->SetupAttachment(root);
	coffinDoorMesh->SetRelativeLocation(FVector(-29.75f, 3.98f, 40.f));
	coffinDoorMesh->SetRelativeRotation(FRotator(0.f, -7.17f, 0.f));

	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dead Body Mesh"));
	bodyMesh->SetupAttachment(coffinMesh);
	bodyMesh->SetRelativeLocation(FVector(0.f, 70.47f, 20.19f));
	bodyMesh->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	jewelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Jewel Mesh"));
	jewelMesh->SetupAttachment(bodyMesh);

	// Open coffin timeline
	TL_Open = CreateDefaultSubobject<UTimelineComponent>(TEXT("Open Coffin Animation"));
	TL_Open->SetTimelineLength(1.f);
}

// Called when the game starts or when spawned
void ACoffin::BeginPlay()
{
	Super::BeginPlay();
	
	SetJewelsCollection(jewels);
	opened = false;
	SetCanBeOpened(false);

	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("SetCoffinDoorRotation") });
	TL_Open->AddInterpFloat(DoorOpenCurve, fcallback);

	tombValue = rand() % 10;

	UpdateMeshes();
}

// Called every frame
void ACoffin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACoffin::SetJewelsCollection(const TArray<UStaticMesh*>& col) {
	if (col.IsEmpty() || col.Num() <= jewelsCollection.Num()) return;

	jewelsCollection.Empty();

	jewelsCollection.Append(col);
}

void ACoffin::ClearJewelsCollection() {
	jewelsCollection.Empty();
}

void ACoffin::SetOutcomeValue(bool negative) {
	negativeOutcome = negative;
}

void ACoffin::SetTombValue(int value) {
	tombValue = FMath::Max(value, 0);
}

void ACoffin::SetCanBeOpened(bool value) {
	canBeOpened = value;

	if (canBeOpened)	Tags.AddUnique("Interactable");
	else				Tags.Remove("Interactable");
}

void ACoffin::OpenCoffin(APlayerCharacter* player) {
	if (not opened && canBeOpened) {
		TL_Open->Play();
		opened = true;
	}
	else if (opened && tombValue > 0) {
		if (player != nullptr)
			player->AddMoney(tombValue);
		tombValue = 0;
		jewelMesh->SetVisibility(false);
		Tags.Remove("Interactable");
	}
}

void ACoffin::SetCoffinDoorRotation(float rot) {
	FRotator currentRotation = coffinDoorMesh->GetRelativeRotation();
	currentRotation.Pitch = rot * COFFIN_OPENED_ROTATION;
	coffinDoorMesh->SetRelativeRotation(currentRotation);
}

void ACoffin::UpdateMeshes() {
	jewelMesh->SetVisibility(tombValue > 0);
	
	if (tombValue > 0 && not jewelsCollection.IsEmpty()) {
		jewelMesh->SetStaticMesh(jewelsCollection[rand() % jewelsCollection.Num()]);
	}
}
