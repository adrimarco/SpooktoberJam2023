// Fill out your copyright notice in the Description page of Project Settings.


#include "Shovel.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"

// Sets default values
AShovel::AShovel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent")));
	auto root = GetRootComponent();

	// Mesh
	shovelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shovel Mesh"));
	shovelMesh->SetupAttachment(root);

	// Dig timeline
	TL_Dig = CreateDefaultSubobject<UTimelineComponent>(TEXT("Dig"));
}

// Called when the game starts or when spawned
void AShovel::BeginPlay()
{
	Super::BeginPlay();

	shovelMesh->SetVisibility(false);
	
	// Bind timeline functions
	FOnTimelineVector vcallback;
	vcallback.BindUFunction(this, FName{ TEXT("SetShovelPosition") });
	TL_Dig->AddInterpVector(PositionCurve, vcallback);

	vcallback.Clear();
	vcallback.BindUFunction(this, FName{ TEXT("SetShovelRotation") });
	TL_Dig->AddInterpVector(RotationCurve, vcallback);

	TL_Dig->SetTimelineLength(2.1f);
}

// Called every frame
void AShovel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShovel::SetShovelPosition(const FVector& pos) {
	shovelMesh->SetRelativeLocation(pos);
}

void AShovel::SetShovelRotation(const FVector& rot) {
	shovelMesh->SetRelativeRotation(rot.ToOrientationQuat());
}

void AShovel::PlayAnimation() {
	TL_Dig->PlayFromStart();
	TL_Dig->SetLooping(true);
	shovelMesh->SetVisibility(true);
}

void AShovel::StopAnimation() {
	TL_Dig->Stop();
	shovelMesh->SetVisibility(false);
}
