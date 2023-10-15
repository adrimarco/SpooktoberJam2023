// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster_AIController.h"
#include "Monster.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"

//Includes for ai perception
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include <Kismet/GameplayStatics.h>

AMonster_AIController::AMonster_AIController(FObjectInitializer const& objInit)
{
	setupPerceptionSystem();
}

void AMonster_AIController::Tick(float DeltaTime)
{
	if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		APlayerCharacter const* p = Cast<APlayerCharacter>(player);
		lampState = p->getLampState();
		GetBlackboardComponent()->SetValueAsBool("LampState", lampState);

		if (!lampState && !trackPlayer) {
			if (timerRandom >= TIME_RANGE_SEARCH_PLAYER) {
				int rand = FMath::RandRange(0, timerLamp);
				//UE_LOG(LogTemp, Warning, TEXT("Rad: %d"), rand);

				GetBlackboardComponent()->SetValueAsBool("TrackPlayer", rand == 0);
				trackPlayer = (rand == 0);
				timerRandom -= TIME_RANGE_SEARCH_PLAYER;
				if(timerLamp > 0)
					timerLamp--;
			}
			else {
				timerRandom += DeltaTime;
			}

			//UE_LOG(LogTemp, Warning, TEXT("Timer: %f"), timerRandom);
		}
		else {
			if (timerRandom > 0)
				timerRandom -= DeltaTime;
			if(timerLamp < MAX_TIMER_LAMP)
				timerLamp++;
		}
	}
}

void AMonster_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AMonster* const m = Cast<AMonster>(InPawn)) {
		if (UBehaviorTree* const tree = m->GetBehaviorTree()) {
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard = b;
			RunBehaviorTree(tree);
		}
	}
}

void AMonster_AIController::setupPerceptionSystem()
{
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (sightConfig) {
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		sightConfig->SightRadius = sight_radius;
		sightConfig->LoseSightRadius = sight_radius + increment_lose_sight_radius;
		sightConfig->PeripheralVisionAngleDegrees = peripheral_vision_angles;
		sightConfig->SetMaxAge(max_age);
		sightConfig->AutoSuccessRangeFromLastSeenLocation = sight_radius ;
		sightConfig->DetectionByAffiliation.bDetectEnemies = true;
		sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*sightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AMonster_AIController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense((*sightConfig));
	}
}

void AMonster_AIController::OnTargetDetected(AActor* actor, FAIStimulus const stimulus)
{
	if (APlayerCharacter* const ch = Cast<APlayerCharacter>(actor)) {
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", stimulus.WasSuccessfullySensed());
		GetBlackboardComponent()->SetValueAsBool("TrackPlayer", false);
	}

}
