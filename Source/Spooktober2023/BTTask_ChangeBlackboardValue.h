// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChangeBlackboardValue.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTTask_ChangeBlackboardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_ChangeBlackboardValue(FObjectInitializer const& objInit);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value", meta = (AllowPrivateAccess = "true"))
	bool value{};
};
