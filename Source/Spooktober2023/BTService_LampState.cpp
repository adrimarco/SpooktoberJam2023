// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_LampState.h"
#include "Monster_AIController.h"
#include "Monster.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_LampState::UBTService_LampState()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Lamp State";
}

void UBTService_LampState::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//get ai controller and enemy
	/*AMonster_AIController const* aiCont = Cast<AMonster_AIController>(OwnerComp.GetAIOwner());
	AMonster const* enemy = Cast<AMonster>(aiCont->GetPawn());*/

	//get player
	ACharacter const* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	APlayerCharacter const* p = Cast<APlayerCharacter>(player);
	if (p != nullptr) {
		//check lamp state and set it in to the bt
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), p->getLampState());
	}
	
}
