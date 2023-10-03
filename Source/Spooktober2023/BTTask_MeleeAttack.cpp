// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster.h"
#include "Monster_AIController.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = "Melee  Attack";
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//check player out of range
	auto const outOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	if (outOfRange) {
		//return success
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	//in range to attack
	auto const* aiCont = OwnerComp.GetAIOwner();
	auto* const enemy = Cast<AMonster>(aiCont->GetPawn());
	if (auto* const icombat = Cast<ICombat_Interface>(enemy)) {
		icombat->Execute_MeleeAttack(enemy);
	}
	//return success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Type();
}
