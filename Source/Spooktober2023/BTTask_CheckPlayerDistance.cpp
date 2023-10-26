// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CheckPlayerDistance.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include "Monster_AIController.h"
#include "Monster.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CheckPlayerDistance::UBTTask_CheckPlayerDistance(FObjectInitializer const& objInit)
{
	NodeName = "CheckPlayerDistance";
}

EBTNodeResult::Type UBTTask_CheckPlayerDistance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {

		AMonster_AIController* aiCont = Cast<AMonster_AIController>(OwnerComp.GetAIOwner());
		AMonster const* enemy = Cast<AMonster>(aiCont->GetPawn());

		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), (enemy->GetDistanceTo(player) > playerInRadius));
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("TrackPlayer", false);
		aiCont->trackPlayer = false;

		//return success
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}


	return EBTNodeResult::Failed;
}
