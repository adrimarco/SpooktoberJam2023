// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_LampState.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTService_LampState : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_LampState();
	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
