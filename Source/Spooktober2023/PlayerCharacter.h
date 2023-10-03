// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

#define CAT_COMPONENTS	"Components"
#define CAT_INPUTS		"Input"
#define CAT_CAMERA		"Camera"
#define CAT_TIMELINE	"Timeline"
#define CAT_LAMP		"Lamp"
#define CAT_SPRINT		"Sprint"

class UStaticMeshComponent;
class UCameraComponent;
class UTimelineComponent;
class UPointLightComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class SPOOKTOBER2023_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= CAT_LAMP)
	bool lightOn{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAT_LAMP)
	float lightIntensity{ 2000.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAT_SPRINT)
	float stamina{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAT_SPRINT)
	bool running{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAT_SPRINT)
	bool exhausted{ false };

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CAT_COMPONENTS)
	UStaticMeshComponent* lampMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CAT_COMPONENTS)
	UCameraComponent* camera{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CAT_COMPONENTS)
	UPointLightComponent* lampLight{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CAT_COMPONENTS)
	UTimelineComponent* TL_TurnLighOn{ nullptr };

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_INPUTS)
	UInputMappingContext* DefaultMappingContext{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_INPUTS)
	UInputAction* MoveAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_INPUTS)
	UInputAction* LookAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_INPUTS)
	UInputAction* LightAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_INPUTS)
	UInputAction* RunAction{ nullptr };

	// Timeline curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_TIMELINE)
	UCurveFloat* LightIntensityCurve{ nullptr };

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_CAMERA)
	TSubclassOf<UCameraShakeBase> StaticShake{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAT_CAMERA)
	TSubclassOf<UCameraShakeBase> MovementShake{ nullptr };

	constexpr UStaticMeshComponent* GetLampMesh()	const { return lampMesh; }
	constexpr UPointLightComponent* GetPointLight()	const { return lampLight; }
	constexpr UCameraComponent*		GetCamera()		const { return camera; }
	constexpr bool					IsRunning()		const { return running; }
	constexpr bool					IsExhausted()	const { return exhausted; }
};
