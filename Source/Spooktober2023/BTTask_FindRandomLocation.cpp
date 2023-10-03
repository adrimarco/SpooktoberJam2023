// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Monster_AIController.h"


UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const&)
{
	NodeName = "Find Random Location In NavMesh";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get AI controller and the enemy
	if (AMonster_AIController* const cont = Cast<AMonster_AIController>(OwnerComp.GetAIOwner())) {

		if (APawn* const m = cont->GetPawn()) {
			
			//get enemy to use as an origin
			FVector const Origin = m->GetActorLocation();

			//get the navigation system and generate a random position
			if (UNavigationSystemV1* const navSys = UNavigationSystemV1::GetCurrent(GetWorld())) {
				
				//get random point
				FNavLocation loc;
				if (navSys->GetRandomPointInNavigableRadius(Origin, searchRadius, loc)) {
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);
				}

				//return success
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}

	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
