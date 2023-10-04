// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UStaticMeshComponent;
class UCameraComponent;
class UTimelineComponent;
class UPointLightComponent;
class UInputMappingContext;
class UInputAction;
class UAIPerceptionStimuliSourceComponent;


UCLASS()
class SPOOKTOBER2023_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UAIPerceptionStimuliSourceComponent* stimulusSource;
	void setupStimulusSource();

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void UpdateStamina(float time);
	UFUNCTION(BlueprintCallable)
	void LightLamp(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void SetLightIntensityFactor(float intensityFactor);
	UFUNCTION(BlueprintCallable)
	void ApplyCameraShake();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Class variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Lamp")
	bool lightOn{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float lightIntensity{ 2000.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float stamina{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	bool running{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	bool exhausted{ false };

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* lampMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* camera{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UPointLightComponent* lampLight{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_TurnLighOn{ nullptr };

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LightAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RunAction{ nullptr };

	// Timeline curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* LightIntensityCurve{ nullptr };

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> StaticShake{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> MovementShake{ nullptr };

	constexpr UStaticMeshComponent* GetLampMesh()	const { return lampMesh; }
	constexpr UPointLightComponent* GetPointLight()	const { return lampLight; }
	constexpr UCameraComponent*		GetCamera()		const { return camera; }
	constexpr bool					IsRunning()		const { return running; }
	constexpr bool					IsExhausted()	const { return exhausted; }
};
