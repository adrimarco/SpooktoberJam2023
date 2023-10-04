// Fill out your copyright notice in the Description page of Project Settings.


#include "GraveGround.h"
#include "Curves/CurveVector.h"
#include "Components/TimelineComponent.h"

constexpr auto NUM_GROUND_MESHES = 10;

// Sets default values
AGraveGround::AGraveGround()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	auto root = GetRootComponent();

	// Initialize meshes and set intial position
	groundMeshes.SetNum(NUM_GROUND_MESHES);
	for (int i = 0; i < NUM_GROUND_MESHES; ++i) {
		FString componentName = "Ground";
		componentName.AppendInt(i);
		groundMeshes[i] = CreateDefaultSubobject<UStaticMeshComponent>(FName(*componentName));
		groundMeshes[i]->SetupAttachment(root);
	}
	SetGroundLocation(FVector(45.f, 45.f, 0.f));

	// Dig timeline
	TL_Dig = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dig Animation"));
	TL_Dig->SetTimelineLength(15.f);
	
}

// Called when the game starts or when spawned
void AGraveGround::BeginPlay()
{
	Super::BeginPlay();
	
	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("SetGroundScale") });
	TL_Dig->AddInterpFloat(GroundScaleCurve, fcallback);

	FOnTimelineVector vcallback;
	vcallback.BindUFunction(this, FName{ TEXT("SetGroundLocation") });
	TL_Dig->AddInterpVector(GroundLocationCurve, vcallback);

	TL_Dig->Play();
}

// Called every frame
void AGraveGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGraveGround::SetGroundLocation(FVector location) {
	if (groundMeshes.Num() < 9) return;

	groundMeshes[0]->SetRelativeLocation({ location.X *0.8	, 0					, location.Z - 4.f });
	groundMeshes[1]->SetRelativeLocation({ -location.X		, 0					, location.Z - 8.f });
	groundMeshes[2]->SetRelativeLocation({ location.X		, location.Y		, location.Z - 12.f });
	groundMeshes[3]->SetRelativeLocation({ -location.X*0.8	, location.Y		, location.Z - 10.f });
	groundMeshes[4]->SetRelativeLocation({ location.X *0.6	, location.Y * 2	, location.Z - 4.f });
	groundMeshes[5]->SetRelativeLocation({ -location.X*0.6	, location.Y * 2	, location.Z - 10.f });
	groundMeshes[6]->SetRelativeLocation({ location.X *0.8	, -location.Y		, location.Z - 8.f });
	groundMeshes[7]->SetRelativeLocation({ -location.X		, -location.Y		, location.Z - 8.f });
	groundMeshes[8]->SetRelativeLocation({ location.X *0.6	, -location.Y * 2	, location.Z - 4.f });
	groundMeshes[9]->SetRelativeLocation({ -location.X*0.6	, -location.Y * 2	, location.Z - 12.f });
}

void AGraveGround::SetGroundScale(float scale) {
	for (auto i = 0; i < NUM_GROUND_MESHES; ++i) {
		groundMeshes[i]->SetRelativeScale3D(FVector(scale, scale, scale));
	}
}
