// Fill out your copyright notice in the Description page of Project Settings.


#include "Grave.h"
#include "GraveGround.h"
#include "Coffin.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"
#include "Components/BoxComponent.h"
#include "Text3DComponent.h"


constexpr auto COFFIN_INIT_Z	= -56.f;
constexpr auto COFFIN_FINAL_Z	= 15.f;

// Sets default values
AGrave::AGrave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	
	Tags.AddUnique("Interactable");

	// Grave mesh
	graveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grave Mesh"));
	graveMesh->SetupAttachment(GetRootComponent());
	graveMesh->ComponentTags.AddUnique("BlockInteraction");

	// Child actors
	graveGroundActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Grave Ground Actor"));
	graveGroundActor->SetupAttachment(GetRootComponent());
	graveGroundActor->SetRelativeLocation(FVector(0.f, 183.6f, -2.f));

	coffinActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Coffin Actor"));
	coffinActor->SetupAttachment(GetRootComponent());
	coffinActor->SetRelativeLocation(FVector(0.f, 191.3f, COFFIN_INIT_Z));
	
	// Dig timeline
	TL_Dig = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dig Animation"));
	TL_Dig->SetTimelineLength(20.f);

	// Box collision
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	boxCollision->SetupAttachment(GetRootComponent());
	boxCollision->SetBoxExtent(FVector(70.f, 150.f, 12.f));
	boxCollision->SetRelativeLocation(FVector(0.f, 166.4f, 0.f));
	boxCollision->SetCollisionResponseToAllChannels(ECR_Block);
	boxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// 3D text
	deadNameText = CreateDefaultSubobject<UText3DComponent>(TEXT("Dead Name 3DText"));
	deadNameText->SetupAttachment(graveMesh);
	deadNameText->SetRelativeLocation(FVector(0.f, 6.376f, 55.65f));
	deadNameText->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	deadNameText->SetText(FText::FromString("Marco\nRobles"));
	deadNameText->SetExtrude(10.f);
	deadNameText->SetHorizontalAlignment(EText3DHorizontalTextAlignment::Center);
	deadNameText->SetVerticalAlignment(EText3DVerticalTextAlignment::Center);
	deadNameText->SetHasMaxWidth(true);
	deadNameText->SetHasMaxHeight(true);
	deadNameText->SetMaxWidth(300.f);
	deadNameText->SetMaxHeight(20.f);
}

void AGrave::OnConstruction(const FTransform& Transform) {
	// Set grave name material
	if (deadNameMaterial != nullptr) {
		deadNameText->SetFrontMaterial(deadNameMaterial);
		deadNameText->SetBackMaterial(deadNameMaterial);
		deadNameText->SetBevelMaterial(deadNameMaterial);
		deadNameText->SetExtrudeMaterial(deadNameMaterial);
	}

	// Create child actors
	if (groundBP == nullptr || coffinBP == nullptr) return;

	graveGroundActor->SetChildActorClass(groundBP);
	coffinActor->SetChildActorClass(coffinBP);
}

// Called when the game starts or when spawned
void AGrave::BeginPlay()
{
	Super::BeginPlay();
	
	if (groundBP == nullptr || coffinBP == nullptr) {
		Destroy();
		return;
	}
	
	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(graveGroundActor->GetChildActor(), FName{TEXT("SetGroundScale")});
	TL_Dig->AddInterpFloat(GroundScaleCurve, fcallback);

	FOnTimelineVector vcallback;
	vcallback.BindUFunction(graveGroundActor->GetChildActor(), FName{TEXT("SetGroundLocation")});
	TL_Dig->AddInterpVector(GroundLocationCurve, vcallback);

	fcallback.Unbind();
	fcallback.BindUFunction(this, FName{ TEXT("SetCoffinHeight") });
	TL_Dig->AddInterpFloat(CoffinPositionCurve, fcallback);

	TL_Dig->SetNewTime(0.f);
	TL_Dig->SetPlayRate(10.f);

	// Bind timeline finish function
	FOnTimelineEventStatic func;
	func.BindUFunction(this, "FinishDigging");
	TL_Dig->SetTimelineFinishedFunc(func);
}

// Called every frame
void AGrave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrave::SetCoffinHeight(float height) {
	FVector currentLocation = coffinActor->GetRelativeLocation();
	currentLocation.Z = COFFIN_INIT_Z + (COFFIN_FINAL_Z - COFFIN_INIT_Z) * height;
	coffinActor->SetRelativeLocation(currentLocation);
}

void AGrave::StartDigging() {
	if (not digged) {
		TL_Dig->Play();
	}
}

void AGrave::StopDigging() {
	if (TL_Dig->IsPlaying()) {
		TL_Dig->Stop();
	}
}

void AGrave::FinishDigging() {
	if (not looted) {
		looted = true;
		Cast<ACoffin>(coffinActor->GetChildActor())->SetCanBeOpened(true);
		Tags.Remove("Interactable");
	}
}

void AGrave::SetDeadName(const FName& name) {
	deadName = name;
	deadNameText->SetText(FText::FromName(name));
}
