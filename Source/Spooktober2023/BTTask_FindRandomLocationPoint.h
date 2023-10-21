// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_FindRandomLocationPoint.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTTask_FindRandomLocationPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	explicit UBTTask_FindRandomLocationPoint(FObjectInitializer const&);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector OriginPosition{};*/



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float searchRadius = 1500.f;
};
