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
	UFUNCTION(BlueprintCallable)
	void LightLamp(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void SetLightIntensityFactor(float intensityFactor);
	UFUNCTION()
	void ApplyCameraShake();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool lightOn{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float lightIntensity{ 2000.f };

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* lampMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* camera{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPointLightComponent* lampLight{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTimelineComponent* TL_TurnLighOn{ nullptr };

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LookAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LightAction{ nullptr };

	// Timeline curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* LightIntensityCurve{ nullptr };

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> StaticShake{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> MovementShake{ nullptr };

	constexpr UStaticMeshComponent* GetLampMesh()	const { return lampMesh; }
	constexpr UCameraComponent*		GetCamera()		const { return camera; }
};
