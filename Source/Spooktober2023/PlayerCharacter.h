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
class ACoffin;
class UUserWidget;

struct PaperMessage {
	FText message;
	FName title;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPaperCollectedDelegate, FText, paperText);

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
	void Interact(const FInputActionValue& Value);
	void StopInteract(const FInputActionValue& Value);
	void OpenCoffin(const ACoffin& coffin);
	void UpdateStamina(float time);
	int32 AddPaperOrdered(const PaperMessage& paper);

	UFUNCTION(BlueprintCallable)
	void LightLamp(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void SetLightIntensityFactor(float intensityFactor);
	UFUNCTION(BlueprintCallable)
	void ApplyCameraShake();
	UFUNCTION(BlueprintCallable)
	void SetMoney(int m);
	UFUNCTION(BlueprintCallable)
	void AddMoney(int m);
	UFUNCTION(BlueprintCallable)
	void ClosePaper();

	UFUNCTION(BlueprintCallable)
	bool getLampState() const noexcept;

	UFUNCTION(BlueprintCallable)
	TArray<FName> getCollectedPapersTitles() const;

	UFUNCTION(BlueprintCallable)
	FText getPaperText(const FName& title) const;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int money{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector interactionDirection{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool blockInput{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool blockMovement{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool blockCamera{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool slowCamera{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool uiWidgetActive{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* paperWidget{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* inventoryWidget{ nullptr };

	TArray<PaperMessage> collectedPapers;
	AActor* interactingWith{ nullptr };

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BackAction{ nullptr };

	// Timeline curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* LightIntensityCurve{ nullptr };

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> StaticShake{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> MovementShake{ nullptr };

	// Event dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FPaperCollectedDelegate OnPaperCollected;

	constexpr UStaticMeshComponent* GetLampMesh()	const { return lampMesh; }
	constexpr UPointLightComponent* GetPointLight()	const { return lampLight; }
	constexpr UCameraComponent*		GetCamera()		const { return camera; }
	constexpr bool					IsRunning()		const { return running; }
	constexpr bool					IsExhausted()	const { return exhausted; }
};
