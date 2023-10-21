// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Ghost_AIController.generated.h"


class AGhost;
/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API AGhost_AIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float angerLevel{};
	


	float timerAnger{0.f};
	float MAX_TIMER_ANGER{5.f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	FVector originPosition{};

	AGhost* ghostActor{};

	bool lampState{};
	bool teleportActorAI{ false };

public:
	explicit AGhost_AIController(FObjectInitializer const& objInit);
	virtual void Tick(float DeltaTime) override;

	void updateOriginPosition(FVector oPos);
	UFUNCTION(BlueprintCallable)
	void teleportActor();
protected:
	virtual void OnPossess(APawn* InPawn) override;

};
