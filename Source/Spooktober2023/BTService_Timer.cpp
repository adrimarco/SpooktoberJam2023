// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Timer.h"
#include "Monster_AIController.h"
#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_Timer::UBTService_Timer()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Service Timer Bool";
}

void UBTService_Timer::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get ai controller and enemy
	AMonster_AIController* aiCont = Cast<AMonster_AIController>(OwnerComp.GetAIOwner());
	AMonster * enemy = Cast<AMonster>(aiCont->GetPawn());
	timerDelegate.BindUFunction(this, "timerEnd", aiCont);

	ACharacter const* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GetWorld()->GetTimerManager().SetTimer(
		timerHandle, // handle to cancel timer at a later time
		timerDelegate, // function to call on elapssed
		MaxTime, // float delay until elapsed
		false, 0.f); // no looping
}

void UBTService_Timer::timerEnd(AMonster_AIController* aiCont)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Your Message"));
	aiCont->GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), actionOnEnd);
}
