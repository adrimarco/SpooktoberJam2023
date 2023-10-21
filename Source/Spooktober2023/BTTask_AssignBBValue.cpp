// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AssignBBValue.h"
#include "BehaviorTree/BlackboardComponent.h"
UBTTask_AssignBBValue::UBTTask_AssignBBValue(FObjectInitializer const& objInit)
{
	NodeName = "Assign Value";
}

EBTNodeResult::Type UBTTask_AssignBBValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FVector v = OwnerComp.GetBlackboardComponent()->GetValueAsVector(valueToAssign.SelectedKeyName);
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), v);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
