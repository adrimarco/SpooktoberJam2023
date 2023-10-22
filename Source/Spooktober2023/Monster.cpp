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
