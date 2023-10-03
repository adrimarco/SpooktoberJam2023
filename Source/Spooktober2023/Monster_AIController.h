// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Perception/AIPerceptionTypes.h>
#include "Monster_AIController.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API AMonster_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	explicit AMonster_AIController(FObjectInitializer const& objInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float sight_radius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float increment_lose_sight_radius = 100.f;

	/** Duration to forgot the stimuli generated (0 -> never forgotten)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float increment_auto_success = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float peripheral_vision_angles = 90.f;

	/** Duration to forgot the stimuli generated (0 -> never forgotten)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float max_age = 7.f;

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	class UAISenseConfig_Sight* sightConfig;

	void setupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);

};
