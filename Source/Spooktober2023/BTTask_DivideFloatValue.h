// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_DivideFloatValue.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTTask_DivideFloatValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	explicit UBTTask_DivideFloatValue(FObjectInitializer const& objInit);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value", meta = (AllowPrivateAccess = "true"))
	float value{2.f};
};
