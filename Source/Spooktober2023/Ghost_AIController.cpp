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
	}
	if (IsInsideRadius) {
		if (timerAnger >= MAX_TIMER_ANGER) {
			if (lampState) {
				angerLevelBB += DeltaTime;
				angerLevel = (angerLevelBB = FMath::Min(angerLevelBB, 1.f));
				
			}
			
			timerAnger = 0.f;
		}
		else {
			timerAnger = FMath::Clamp(timerAnger + DeltaTime, 0.f, MAX_TIMER_ANGER);
		}
	}

	GetBlackboardComponent()->SetValueAsFloat("AngerLevel", angerLevelBB);
	ghostActor->setSphereCollisionRadius(FMath::Lerp(ghostActor->minSightRadius, ghostActor->maxSightRadius, angerLevelBB));
	float maxSpeed = (IsInsideRadius) ? ghostActor->maxSpeed : FMath::Min(ghostActor->speed, ghostActor->wanderSpeed);
	ghostActor->setSpeedGhost(FMath::Lerp(ghostActor->minSpeed, maxSpeed, angerLevelBB));
	UE_LOG(LogTemp, Warning, TEXT("Anger Level: %f"), angerLevelBB);
}

void AGhost_AIController::updateOriginPosition(FVector oPos)
{
	originPosition = oPos;
	GetBlackboardComponent()->SetValueAsVector("OriginPosition", oPos);
}

void AGhost_AIController::teleportActor() {

	if (!ghostActor->TL_Opacity->IsReversing()) {
		FVector target = GetBlackboardComponent()->GetValueAsVector("OriginPosition");
		ghostActor->SetActorLocation(target);
		ghostActor->TL_Opacity->Reverse();
		//ghostActor->TL_Opacity->SetPlaybackPosition(0.f, false);
	}
	
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
			RunBehaviorTree(tree);
		}
	}
}