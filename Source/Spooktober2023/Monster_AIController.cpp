// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster_AIController.h"
#include "Monster.h"
#include "Spooktober2023Character.h"
#include "BehaviorTree/BlackboardComponent.h"

//Includes for ai perception
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMonster_AIController::AMonster_AIController(FObjectInitializer const& objInit)
{
	setupPerceptionSystem();
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
	if (ASpooktober2023Character* const ch = Cast<ASpooktober2023Character>(actor)) {
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", stimulus.WasSuccessfullySensed());
	}

}
