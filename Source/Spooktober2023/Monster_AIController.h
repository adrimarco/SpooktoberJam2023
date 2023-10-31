// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Perception/AIPerceptionTypes.h>
#include "PlayerCharacter.h"
#include "Monster_AIController.generated.h"


class AMonster;
/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API AMonster_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	explicit AMonster_AIController(FObjectInitializer const& objInit);
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float sight_radius = 2000.f;

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

	bool trackPlayer = false;
	AMonster* monsterPawn{ nullptr };

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	class UAISenseConfig_Sight* sightConfig;
	bool lampState = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	float TIME_RANGE_SEARCH_PLAYER = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	int MAX_TIMER_LAMP = 5;

	float timerRandom = 0.f;
	int timerLamp = MAX_TIMER_LAMP;

	void setupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* actor, FAIStimulus const stimulus);

};
