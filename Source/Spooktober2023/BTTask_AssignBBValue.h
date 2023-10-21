// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AssignBBValue.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTTask_AssignBBValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	explicit UBTTask_AssignBBValue(FObjectInitializer const& objInit);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector valueToAssign{};
};
