// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_ChangeBlackboardValue::UBTTask_ChangeBlackboardValue(FObjectInitializer const& objInit)
{
	NodeName = "Change Value BlackBoard";
}

EBTNodeResult::Type UBTTask_ChangeBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), value);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
