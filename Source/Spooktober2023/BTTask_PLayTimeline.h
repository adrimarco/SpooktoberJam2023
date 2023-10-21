// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_PLayTimeline.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTTask_PLayTimeline : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	

public:
	explicit UBTTask_PLayTimeline(FObjectInitializer const&);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeLine")
	TSubclassOf<APawn> pawnClass{nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TimeLine")
	bool reverseAnimation{false};
};
