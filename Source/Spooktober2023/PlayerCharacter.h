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
class AShovel;
class UNiagaraSystem;
class UNiagaraComponent;

struct PaperMessage {
	FText message;
	FName title;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPaperCollectedDelegate, FText, paperText);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmblemCollectedDelegate, int, emblemId);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoneyIncreasedDelegate, int, moneyAdded, int, totalMoney);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnterSecureZoneDelegate, bool, enter);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeadDelegate);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerEndDeadDelegate);

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
	void PlayStepsSound();
	void UpdateStepsSound();
	void CheckFloorMaterial();
	int32 AddPaperOrdered(const PaperMessage& paper);

	UFUNCTION(BlueprintCallable)
	void setDeadAnimation(const FVector& rot);

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
	void extinguishLamp(float time = 60.f);
	UFUNCTION(BlueprintCallable)
	void igniteLamp();

	UFUNCTION(BlueprintCallable)
	TArray<FName> getCollectedPapersTitles() const;

	UFUNCTION(BlueprintCallable)
	FText getPaperText(const FName& title) const;

	UFUNCTION(BlueprintCallable)
	void SetMausoleumNumber(int size);

	UFUNCTION(BlueprintCallable)
	void EmblemCollected(int id);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<int> GetCollectedEmblems();

	UFUNCTION(BlueprintCallable)
	void StopDigging();

	void enterSecureZone(bool enterArea);
	void decreaseHealth(int damage);
	void endDeadAnimation();


	UFUNCTION(BlueprintCallable)
	void exitToMenu();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Class variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int health{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Lamp")
	bool lightOn{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	bool blockLamp{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float lightIntensity{ 2000.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float fireSize{ 0.4f };

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* messageWidget{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* screenWidget{ nullptr };

	TArray<PaperMessage> collectedPapers;
	AActor* interactingWith{ nullptr };
	TArray<int> collectedEmblems;
	FTimerHandle stepsSoundHandle;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* lampMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* camera{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UPointLightComponent* lampLight{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UNiagaraSystem* lampFireParticles;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UNiagaraComponent* niagaraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_TurnLighOn{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_Dead{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* stepsSound{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* lampSound{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* shovelActor{ nullptr };

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveVector* DeadAnimationCurve{ nullptr };

	// Camera shake
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> StaticShake{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> MovementShake{ nullptr };

	// Blueprint classes for child actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components Blueprint")
	TSubclassOf<AShovel> shovelBP{ nullptr };

	// Event dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FPaperCollectedDelegate OnPaperCollected;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FEmblemCollectedDelegate OnEmblemCollected;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FMoneyIncreasedDelegate OnMoneyIncrease;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FEnterSecureZoneDelegate OnEnterSecureArea;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FPlayerDeadDelegate OnPlayerDead;

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FPlayerEndDeadDelegate OnEndDeadAnimation;

	

	constexpr UStaticMeshComponent* GetLampMesh()	const { return lampMesh; }
	constexpr UPointLightComponent* GetPointLight()	const { return lampLight; }
	constexpr UCameraComponent*		GetCamera()		const { return camera; }
	constexpr bool					IsRunning()		const { return running; }
	constexpr bool					IsExhausted()	const { return exhausted; }
};
