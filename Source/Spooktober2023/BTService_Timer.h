// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "TimerManager.h"
#include "BTService_Timer.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UBTService_Timer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float MaxTime{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	bool actionOnEnd{};

	FTimerHandle timerHandle;
	FTimerDelegate timerDelegate; // Delegate to bind function with parameters
	

public:
	UBTService_Timer();
	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void timerEnd(AMonster_AIController* aiCont);
};
