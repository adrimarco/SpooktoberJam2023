// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPlayerLocation.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include <NavigationSystem.h>
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& objInit)
{
	NodeName = "Find Player Location";
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get player character
	if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {

		//get player location to use as origin
		FVector const playerLocation = player->GetActorLocation();
		if (searchRandom) {
			FNavLocation loc;

			//get the navigation system and generate a random location near the player
			if (UNavigationSystemV1* const navSys = UNavigationSystemV1::GetCurrent(GetWorld())) {
				//try to get a random location near the player
				if (navSys->GetRandomPointInNavigableRadius(playerLocation, searchRadius, loc)) {

					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);

					//return success
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}else{

			OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), playerLocation);

			//return success
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;
		}

	}

	return EBTNodeResult::Failed;
}
