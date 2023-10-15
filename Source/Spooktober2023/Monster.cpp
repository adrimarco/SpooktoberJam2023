// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Monster_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	UE_LOG(LogTemp, Warning, TEXT("Putaso    aaaaaaa"));

	return 0;
}
