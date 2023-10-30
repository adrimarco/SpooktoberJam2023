// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DivideFloatValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DivideFloatValue::UBTTask_DivideFloatValue(FObjectInitializer const& objInit)
{
	NodeName = "Divide Float Value";
}

EBTNodeResult::Type UBTTask_DivideFloatValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float angerValue = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(GetSelectedBlackboardKey());
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(GetSelectedBlackboardKey(), angerValue/value);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
