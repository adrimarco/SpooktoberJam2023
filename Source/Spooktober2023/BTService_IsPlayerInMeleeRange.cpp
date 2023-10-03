// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsPlayerInMeleeRange.h"
#include "Monster_AIController.h"
#include "Monster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_IsPlayerInMeleeRange::UBTService_IsPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Is player In Melee Range";
}

void UBTService_IsPlayerInMeleeRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get ai controller and enemy
	AMonster_AIController const* aiCont = Cast<AMonster_AIController>(OwnerComp.GetAIOwner());
	AMonster const* enemy = Cast<AMonster>(aiCont->GetPawn());

	//get player
	ACharacter const* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//check if the player is in range and set it in to the bt
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), enemy->GetDistanceTo(player) <= meleeRange);
}
