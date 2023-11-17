// Fill out your copyright notice in the Description page of Project Settings.

#include "Ghost_AIController.h"
#include "Ghost.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Components/TimelineComponent.h"

AGhost_AIController::AGhost_AIController(FObjectInitializer const& objInit)
{
	
}

void AGhost_AIController::Tick(float DeltaTime)
{

	bool IsInsideRadius = GetBlackboardComponent()->GetValueAsBool("InsideRadius");
	float angerLevelBB = GetBlackboardComponent()->GetValueAsFloat("AngerLevel");
	if (ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) {
		APlayerCharacter const* p = Cast<APlayerCharacter>(player);
		lampState = p->getLampState();
		GetBlackboardComponent()->SetValueAsBool("LampState", lampState);

		if (IsInsideRadius) {
			if (timerAnger >= MAX_TIMER_ANGER) {
				if (lampState) {
					angerLevelBB += MAX_TIMER_ANGER/100;
					angerLevel = (angerLevelBB = FMath::Min(angerLevelBB, 1.f));

				}

				timerAnger -= MAX_TIMER_ANGER;
			}
			timerAnger += DeltaTime;
		}

		GetBlackboardComponent()->SetValueAsFloat("AngerLevel", angerLevelBB);
		ghostActor->setSphereCollisionRadius(FMath::Lerp(ghostActor->minSightRadius, ghostActor->maxSightRadius, angerLevelBB));

		
		if ((IsInsideRadius && lampState)) {
			ghostActor->setSpeedGhost(FMath::Lerp(ghostActor->minSpeed, ghostActor->maxSpeed, angerLevelBB));
		}
		else {
			if (ghostActor->speed > ghostActor->wanderSpeed) {
				ghostActor->setSpeedGhost(FMath::Max(ghostActor->speed - DeltaTime * 100, ghostActor->wanderSpeed));
			}
		}
	} 
}



void AGhost_AIController::updateOriginPosition(FVector oPos)
{
	originPosition = oPos;
	GetBlackboardComponent()->SetValueAsVector("OriginPosition", oPos);
}

void AGhost_AIController::teleportActor() {

	if (ghostActor->teleportActorAI) {
		ghostActor->teleportActorAI = false;
		ghostActor->collidePlayerOnce = true;
		FVector target = GetBlackboardComponent()->GetValueAsVector("OriginPosition");
		ghostActor->SetActorLocation(target);
		ghostActor->TL_Opacity->Reverse();
	}
}

void AGhost_AIController::increaseAngerLevel(float increaseFactor)
{
	float angerLevelBB = GetBlackboardComponent()->GetValueAsFloat("AngerLevel");
	GetBlackboardComponent()->SetValueAsFloat("AngerLevel", FMath::Min(angerLevelBB + increaseFactor, 1.f));
}

void AGhost_AIController::startBloodyMoon()
{
	angerLevel = 1.f;
	GetBlackboardComponent()->SetValueAsFloat("AngerLevel", 1.f);
	GetBlackboardComponent()->SetValueAsBool("IsBloodyMoon", true);
}


//Initialize ai controller values
void AGhost_AIController::initializeAIController()
{
	GetBlackboardComponent()->SetValueAsBool("IsBloodyMoon", false);
	angerLevel = 0.f;
	GetBlackboardComponent()->SetValueAsFloat("AngerLevel", 0.f);
	teleportActorAI = false;
}

void AGhost_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AGhost* const g = Cast<AGhost>(InPawn)) {
		ghostActor = g;
		
		if (UBehaviorTree* const tree = g->GetBehaviorTree()) {
			UBlackboardComponent* b;
			UseBlackboard(tree->BlackboardAsset, b);
			Blackboard = b;
			updateOriginPosition(ghostActor->GetActorLocation());
			FOnTimelineEventStatic finishCallback;
			finishCallback.BindUFunction(this, FName{ TEXT("teleportActor") });
			ghostActor->TL_Opacity->SetTimelineFinishedFunc(finishCallback);
			initializeAIController();
			RunBehaviorTree(tree);
		}
	}
}