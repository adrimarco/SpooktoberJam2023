// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomLocationPoint.h"
#include "Ghost_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"

UBTTask_FindRandomLocationPoint::UBTTask_FindRandomLocationPoint(FObjectInitializer const&)
{
	NodeName = "Find Random Position in NavMesh from a Point";
}

EBTNodeResult::Type UBTTask_FindRandomLocationPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get AI controller and the enemy
	AAIController* const cont = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (cont != nullptr) {
	
		//get enemy to use as an origin
		FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector("OriginPosition");
		UE_LOG(LogTemp, Warning, TEXT("Origin"));

		//get the navigation system and generate a random position
		if (UNavigationSystemV1* const navSys = UNavigationSystemV1::GetCurrent(GetWorld())) {
			
			//get random point
			FNavLocation loc{};
			if (navSys->GetRandomPointInNavigableRadius(Origin, searchRadius, loc)) {
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), loc.Location);
				UE_LOG(LogTemp, Warning, TEXT("Origin:asdasdasd"));
				//return success
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}

			//return success
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return EBTNodeResult::Failed;
		}

	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
