// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TeleportActor.h"
#include <Kismet/GameplayStatics.h>
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_TeleportActor::UBTTask_TeleportActor(FObjectInitializer const&)
{
	NodeName = "Teleport Actor";
}

EBTNodeResult::Type UBTTask_TeleportActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* aiCont = Cast<AAIController>(OwnerComp.GetAIOwner());
	APawn* enemy = aiCont->GetPawn();

	FVector target = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());
	enemy->SetActorLocation(target);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool("TeleportActor", false);
	//return success
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
