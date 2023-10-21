// Fill out your copyright notice in the Description page of Project Settings.


#include "MausoleumDoor.h"
#include "Text3DComponent.h"
#include "Components/TimelineComponent.h"

#define EMBLEM_PLACED_POSITION	FVector(-42.f, -5.1f, 162.f)
#define EMBLEM_INITIAL_Y		-10.f
#define FINAL_DOOR_ROTATION		105.f

// Sets default values
AMausoleumDoor::AMausoleumDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	auto root = GetRootComponent();

	Tags.AddUnique("Interactable");

	// Mesh components
	doorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	doorMesh->SetupAttachment(root);
	doorMesh->SetRelativeLocation(FVector(45.f, 0.f, 0.f));

	secondDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Second Door Mesh"));
	secondDoorMesh->SetupAttachment(root);
	secondDoorMesh->SetRelativeLocation(FVector(-132.5f, 0.f, 0.f));
	secondDoorMesh->SetRelativeScale3D(FVector(-1.f, 1.f, 1.f));

	emblemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Emblem Mesh"));
	emblemMesh->SetupAttachment(doorMesh);
	emblemMesh->SetRelativeLocation(EMBLEM_PLACED_POSITION);
	emblemMesh->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	emblemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	secondEmblemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Second Emblem Mesh"));
	secondEmblemMesh->SetupAttachment(secondDoorMesh);
	secondEmblemMesh->SetRelativeLocation(EMBLEM_PLACED_POSITION);
	secondEmblemMesh->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	secondEmblemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3D Text component
	mausoleumFamiliyText = CreateDefaultSubobject<UText3DComponent>(TEXT("Mausoleum 3DText"));
	mausoleumFamiliyText->SetupAttachment(root);
	mausoleumFamiliyText->SetRelativeLocation(FVector(-42.5f, -206.f, 490.f));
	mausoleumFamiliyText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	mausoleumFamiliyText->SetText(FText::FromString("Sanchez"));
	mausoleumFamiliyText->SetExtrude(3.f);
	mausoleumFamiliyText->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
	mausoleumFamiliyText->SetVerticalAlignment(EText3DVerticalTextAlignment::Center);
	mausoleumFamiliyText->SetHasMaxWidth(true);
	mausoleumFamiliyText->SetMaxWidth(300.f);

	// Timelines
	TL_PlaceEmblem = CreateDefaultSubobject<UTimelineComponent>(TEXT("Place Emblem Animation"));
	TL_PlaceEmblem->SetTimelineLength(1.f);

	TL_OpenDoor = CreateDefaultSubobject<UTimelineComponent>(TEXT("Open Door Animation"));
	TL_OpenDoor->SetTimelineLength(1.f);
}

// Called when the game starts or when spawned
void AMausoleumDoor::BeginPlay()
{
	Super::BeginPlay();
	

	emblemsCollected		= 0;
	doorOpened				= false;
	firstEmblemPlaced		= false;
	secondEmblemPlaced		= false;

	emblemMesh->SetVisibility(false);
	secondEmblemMesh->SetVisibility(false);

	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("UpdateEmblemLocation") });
	TL_PlaceEmblem->AddInterpFloat(PlaceEmblemCurve, fcallback);

	FOnTimelineEventStatic ecallback;
	ecallback.BindUFunction(this, FName{ TEXT("EmblemPlacedAnimationFinished") });
	TL_PlaceEmblem->SetTimelineFinishedFunc(ecallback);

	fcallback.Clear();
	fcallback.BindUFunction(this, FName{ TEXT("UpdateDoorsRotation") });
	TL_OpenDoor->AddInterpFloat(DoorOpenCurve, fcallback);
}

void AMausoleumDoor::OnConstruction(const FTransform& Transform) {
	// Set mausoleum name
	SetMausoleumName(mausoleumName);
}

// Called every frame
void AMausoleumDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMausoleumDoor::InteractDoor(UActorComponent* component) {
	if (component == doorMesh || component == secondDoorMesh) {
		// Interacting with doors, check if emblems are placed
		if (firstEmblemPlaced && secondEmblemPlaced) {
			// Open door
			TL_OpenDoor->PlayFromStart();
			Tags.Remove("Interactable");
		}
		else if (component == doorMesh) {
			// Try to place first emblem
			if (emblemsCollected>0 && not firstEmblemPlaced) {

				PlaceEmblem(true);

			}
		}
		else if (component == secondDoorMesh) {
			// Try to place second emblem
			if (emblemsCollected>0 && not secondEmblemPlaced) {

				PlaceEmblem(false);

			}
		}
	}
}

void AMausoleumDoor::PlaceEmblem(bool first) {
	if (TL_PlaceEmblem->IsPlaying()) return;

	// Select emblem to be placed
	if (first) {
		selectedEmblem = emblemMesh;
		firstEmblemPlaced = true;
	}
	else {
		selectedEmblem = secondEmblemMesh;
		secondEmblemPlaced = true;
	}

	// Show emblem and play animation
	selectedEmblem->SetVisibility(true);
	TL_PlaceEmblem->PlayFromStart();

	// Reduce emblems collected number
	emblemsCollected--;
}

void AMausoleumDoor::UpdateEmblemLocation(float factor) {
	if (selectedEmblem == nullptr) return;

	FVector pos = EMBLEM_PLACED_POSITION;

	pos.Y = EMBLEM_INITIAL_Y + (pos.Y - EMBLEM_INITIAL_Y) * factor;

	selectedEmblem->SetRelativeLocation(pos);
}

void AMausoleumDoor::EmblemPlacedAnimationFinished() {
	selectedEmblem = nullptr;
}

void AMausoleumDoor::UpdateDoorsRotation(float factor) {
	float newYaw{ FINAL_DOOR_ROTATION * factor };

	FRotator rot{ doorMesh->GetRelativeRotation() };
	rot.Yaw = -newYaw;
	doorMesh->SetRelativeRotation(rot);

	rot = secondDoorMesh->GetRelativeRotation();
	rot.Yaw = newYaw;
	secondDoorMesh->SetRelativeRotation(rot);
}

void AMausoleumDoor::EmblemFound(int id) {
	if (id == emblemId) emblemsCollected++;
}

void AMausoleumDoor::SetMausoleumName(const FName& newName) {
	mausoleumName = newName;
	mausoleumFamiliyText->SetText(FText::FromName(newName));
}
