// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include <Kismet/KismetMathLibrary.h>
#include "Monster.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& objInit)
{
	NodeName = "Chase Player";
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get target from the ai controller
	if (AAIController* const cont = Cast<AAIController>(OwnerComp.GetAIOwner())) {
		FVector const pLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

		//move enemy to the location
		auto* const enemy = Cast<AMonster>(cont->GetPawn());
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(cont, pLocation);
		//return success
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}
