// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AssignFloat.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_AssignFloat::UBTTask_AssignFloat(FObjectInitializer const& objInit)
{
	NodeName = "Assign Float to BB Value";
}

EBTNodeResult::Type UBTTask_AssignFloat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float changeValue = value;
	if (useBBValue)
		changeValue = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(bbKey.SelectedKeyName);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(GetSelectedBlackboardKey(), changeValue);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
