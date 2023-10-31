// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghost.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Monster_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/AudioComponent.h"


// Sets default values
AGhost::AGhost()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	sphereCollider->SetupAttachment(RootComponent, NAME_None);
	sphereCollider->SetGenerateOverlapEvents(true);
	sphereCollider->SetSphereRadius(sphereRadius);
	sphereCollider->SetCollisionProfileName(TEXT("Trigger"), false);

	//Init damage collider
	damageCollider = CreateDefaultSubobject<USphereComponent>(TEXT("AttackCollider"));
	damageCollider->SetupAttachment(RootComponent, NAME_None);
	damageCollider->SetGenerateOverlapEvents(true);
	damageCollider->SetSphereRadius(150.f);
	damageCollider->SetCollisionProfileName(TEXT("Trigger"), false);

	idleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Idle Sound"));
	idleSound->SetupAttachment(GetRootComponent());
	idleSound->bAutoActivate = true;

	puffSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Puff Sound"));
	puffSound->SetupAttachment(GetRootComponent());
	puffSound->bAutoActivate = false;

	//Timeline
	TL_Opacity = CreateDefaultSubobject<UTimelineComponent>(TEXT("Change Opacity Animation"));
	TL_Opacity->SetTimelineLength(1.f);
}

// Called when the game starts or when spawned
void AGhost::BeginPlay()
{
	Super::BeginPlay();
	
	//Create material
	dynamicMaterial = UMaterialInstanceDynamic::Create(materialGhost, this);
	GetMesh()->SetMaterial(0, dynamicMaterial);
	 
	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("setMaterialOpacity") });
	TL_Opacity->AddInterpFloat(OpacityCurve, fcallback);

	


}

// Called every frame
void AGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGhost::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	sphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AGhost::OnVolumeBeginOverlap);
	sphereCollider->OnComponentEndOverlap.AddDynamic(this, &AGhost::OnVolumeEndOverlap);

	damageCollider->OnComponentBeginOverlap.AddDynamic(this, &AGhost::OnAttackCollisionBeginOverlap);
	damageCollider->OnComponentEndOverlap.AddDynamic(this, &AGhost::OnAttackCollisionEndOverlap);
	aiController = Cast<AAIController>(GetController());
}

UBehaviorTree* AGhost::GetBehaviorTree() const noexcept
{
	return bTree;
}

void AGhost::setSphereCollisionRadius(float radius)
{
	sphereRadius = radius;
	sphereCollider->SetSphereRadius(sphereRadius);
}

void AGhost::setSpeedGhost(float s)
{
	speed = s;
	UCharacterMovementComponent* moveComp = GetCharacterMovement();
	moveComp->MaxWalkSpeed = speed;
}

UTimelineComponent* AGhost::getTimelineAnimation()
{
	return TL_Opacity;
}


void AGhost::setMaterialOpacity(float op)
{
	//UE_LOG(LogTemp, Warning, TEXT("Opacity: %f"), op);
	dynamicMaterial->SetScalarParameterValue("Opacity", op);
	idleSound->SetVolumeMultiplier(op);
}


void AGhost::playerEnteredSecureArea(bool playerEntered)
{
	AAIController* aiCont = Cast<AAIController>(GetController());
	if (playerEntered) {
		aiCont->GetBlackboardComponent()->SetValueAsBool("LampState", false);
		aiCont->GetBlackboardComponent()->SetValueAsBool("TrackPlayer", false);
	}
	else {

		if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
			APlayerCharacter const* p = Cast<APlayerCharacter>(player);
			aiCont->GetBlackboardComponent()->SetValueAsBool("LampState", p->getLampState());
		}

	}
}


void AGhost::OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		AAIController* aiCont = Cast<AAIController>(GetController());
		aiCont->GetBlackboardComponent()->SetValueAsBool("InsideRadius", true);
	}
}

void AGhost::OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		AAIController* aiCont = Cast<AAIController>(GetController());
		aiCont->GetBlackboardComponent()->SetValueAsBool("InsideRadius", false);
	}
}

void AGhost::OnAttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor)) {
		if (ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
			APlayerCharacter* p = Cast<APlayerCharacter>(player);
			teleportActorAI = true;
			p->extinguishLamp();
			puffSound->Play();
			OnCallBigMonster.Broadcast();
			aiController->GetBlackboardComponent()->SetValueAsBool("Flee", true);
		}
	}
	
}

void AGhost::OnAttackCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}