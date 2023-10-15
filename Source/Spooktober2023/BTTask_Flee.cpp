// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Flee.h"
#include "Monster_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_Flee::UBTTask_Flee(FObjectInitializer const& objInit)
{
	NodeName = "Flee Player";
}

EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get target from the ai controller
	if (AMonster_AIController* const cont = Cast<AMonster_AIController>(OwnerComp.GetAIOwner())) {
		FVector const pLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

		//move enemy to the location
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(cont, pLocation);

		//return success
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
