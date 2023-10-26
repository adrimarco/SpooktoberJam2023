// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindLocationFlee.h"

#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/GameplayStatics.h>
#include "NavigationSystem.h"
#include "Monster_AIController.h"
#include "Math/Vector.h"

UBTTask_FindLocationFlee::UBTTask_FindLocationFlee(FObjectInitializer const& objInit)
{
	NodeName = "Get Random Position Flee";
}

EBTNodeResult::Type UBTTask_FindLocationFlee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
				if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
					FVector const playerLocation = player->GetActorLocation();
					bool validPosition{ false };
					while (!validPosition) {
						
						if (navSys->GetRandomPointInNavigableRadius(Origin, searchExternalRadius, loc)) {
							double dist = FVector::Dist(playerLocation, loc.Location);
							if (dist >= searchInnerRadius)
								validPosition = true;

						}
					}
					if (validPosition) {
						OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);
						//OwnerComp.GetBlackboardComponent()->SetValueAsBool("attackSuccess", false);
					}

					//return success
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				return EBTNodeResult::Failed;
			}
		}

	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
