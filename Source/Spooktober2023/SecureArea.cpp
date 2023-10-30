// Fill out your copyright notice in the Description page of Project Settings.


#include "SecureArea.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"


// Sets default values
ASecureArea::ASecureArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	hitBoxSecureArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	hitBoxSecureArea->SetupAttachment(RootComponent, NAME_None);
	hitBoxSecureArea->SetGenerateOverlapEvents(true);
	hitBoxSecureArea->SetBoxExtent(FVector{10.0, 10.0 , 10.0});
	hitBoxSecureArea->SetCollisionProfileName(TEXT("Trigger"), false);

}

// Called when the game starts or when spawned
void ASecureArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASecureArea::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	hitBoxSecureArea->OnComponentBeginOverlap.AddDynamic(this, &ASecureArea::OnCollisionBeginOverlap);
	hitBoxSecureArea->OnComponentEndOverlap.AddDynamic(this, &ASecureArea::OnCollisionEndOverlap);
}

// Called every frame
void ASecureArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASecureArea::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor)) {
		APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
		player->enterSecureZone(true);
	}
}

void ASecureArea::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor)) {
		APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
		player->enterSecureZone(false);
	}
}

