// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Monster_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"


// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Lamp light
	lampLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Lamp Spot Light"));
	lampLight->SetupAttachment( GetMesh());
	lampLight->SetLightColor(FLinearColor(1.f, 0.f, 0.f));
	lampLight->SetRelativeLocation(FVector(0.f, 50.f, 308.f));
	lampLight->SetRelativeRotation(FRotator(-45.f, 90.f, 0.f));
	lampLight->SetIntensity(2500.f);
	lampLight->SetInnerConeAngle(30.f);
	lampLight->SetOuterConeAngle(35.f);

	attackCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack collider"));
	attackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackCollider->SetupAttachment(GetMesh());
	attackCollider->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	attackCollider->SetRelativeLocation(FVector(0.f, 100.f, 200.f));
	attackCollider->SetRelativeRotation(FRotator(0.f, 0.f, 20.f));


	idleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Idle Sound"));
	idleSound->SetupAttachment(GetRootComponent());
	idleSound->bAutoActivate = true;

	attackSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Attack Sound"));
	attackSound->SetupAttachment(GetRootComponent());
	attackSound->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	attackCollider->OnComponentBeginOverlap.AddDynamic(this, &AMonster::OnAttackCollisionBeginOverlap);
	attackCollider->OnComponentEndOverlap.AddDynamic(this, &AMonster::OnAttackCollisionEndOverlap);
}

UBehaviorTree* AMonster::GetBehaviorTree() const noexcept
{
	return bTree;
}

int AMonster::MeleeAttack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Putaso"));

	//Check player collision

	AAIController* aiCont = Cast<AAIController>(GetController());
	aiCont->GetBlackboardComponent()->SetValueAsBool("attackSuccess", true);
	
	

	return 0;
}

void AMonster::activateAttackCollision(bool activate) {

	ECollisionEnabled::Type collisionState;
	if(activate) collisionState = ECollisionEnabled::QueryAndPhysics;
	else collisionState = ECollisionEnabled::NoCollision;

	attackCollider->SetCollisionEnabled(collisionState);
}

void AMonster::playerEnteredSecureArea(bool playerEntered)
{
	AAIController* aiCont = Cast<AAIController>(GetController());
	if (playerEntered) {
		aiCont->GetBlackboardComponent()->SetValueAsBool("LampState", true);
		aiCont->GetBlackboardComponent()->SetValueAsBool("TrackPlayer", false);
		aiCont->GetBlackboardComponent()->SetValueAsBool("attackSuccess", true);
	}
	else {

		if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
			APlayerCharacter const* p = Cast<APlayerCharacter>(player);
			aiCont->GetBlackboardComponent()->SetValueAsBool("LampState", p->getLampState());
		}
		
		aiCont->GetBlackboardComponent()->SetValueAsBool("attackSuccess", false);
	}
}


void AMonster::OnAttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerCharacter>(OtherActor)) {
		if (ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
			APlayerCharacter* p = Cast<APlayerCharacter>(player);
			//Kill player

			UE_LOG(LogTemp, Warning, TEXT("Putaso    aaaaaaa"));
			attackSound->Play();
		}
	}

}

void AMonster::OnAttackCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}