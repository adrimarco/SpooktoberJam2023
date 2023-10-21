// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TeleportActor.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTTask_TeleportActor : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	explicit UBTTask_TeleportActor(FObjectInitializer const&);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
