// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PLayTimeline.h"
#include "Components/TimelineComponent.h"
#include "Ghost.h"
#include "Monster.h"
#include "Monster_AIController.h"
#include "Ghost_AIController.h"
UBTTask_PLayTimeline::UBTTask_PLayTimeline(FObjectInitializer const&)
{
	NodeName = "Play Animation TimeLine";
}




EBTNodeResult::Type UBTTask_PLayTimeline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	/*UTimelineComponent* timelineAnimation = pawn->getTimelineAnimation();
	if (!reverseAnimation) timelineAnimation->Play();
	else timelineAnimation->Reverse();*/

	AAIController* aiCont = OwnerComp.GetAIOwner();
	if (aiCont->IsA<AGhost_AIController>()) {

		AGhost_AIController* a = Cast<AGhost_AIController>(aiCont);
		AGhost* actor = Cast<AGhost>(a->GetPawn());
		UTimelineComponent* anim = actor->getTimelineAnimation();

		if (!reverseAnimation) anim->Play();
		else anim->Reverse();
	}else if (aiCont->IsA<AMonster_AIController>()) {
		AMonster_AIController* a = Cast<AMonster_AIController>(aiCont);
		AMonster* actor = Cast<AMonster>(a->GetPawn());
		/*AMonster* actor = Cast<AMonster>(a);
		UTimelineComponent* anim = actor->getTimelineAnimation();

		if (!reverseAnimation) anim->Play();
		else anim->Reverse();*/
	}
		

	


	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
	
}
