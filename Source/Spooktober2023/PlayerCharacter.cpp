// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"



constexpr auto LIGHT_INTENSITY			= 5000.f;
constexpr auto LIGHT_ATTENUATION_RADIUS = 2000.f;
constexpr auto MAX_STAMINA				= 10.f;
constexpr auto MIN_RUN_STAMINA			= 5.f;
constexpr auto STAMINA_RECOVER_RATE		= 0.8f;
constexpr auto WALK_SPEED				= 400.f;
constexpr auto RUN_SPEED				= 800.f;


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
	stamina = MAX_STAMINA;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.f);

	// Camera
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	camera->SetupAttachment(GetCapsuleComponent());
	camera->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	camera->bUsePawnControlRotation = true;

	// Camera post process settings
	FPostProcessSettings postProcessSettings{};
	postProcessSettings.bOverride_MotionBlurAmount = true;
	postProcessSettings.bOverride_MotionBlurMax = true;
	postProcessSettings.MotionBlurAmount = 0.2;
	postProcessSettings.MotionBlurMax = 20;

	camera->PostProcessSettings = postProcessSettings;

	// Lamp mesh
	lampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lamp"));
	lampMesh->SetupAttachment(GetCapsuleComponent());
	lampMesh->SetRelativeLocation(FVector(26.f, -18.f, 30.f));
	lampMesh->SetRelativeRotation(FRotator(0.f, 70.f, 0.f));

	// Lamp light
	lampLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Lamp Point Light"));
	lampLight->SetupAttachment(lampMesh);
	lampLight->SetAttenuationRadius(LIGHT_ATTENUATION_RADIUS);
	lightIntensity = LIGHT_INTENSITY;
	lampLight->SetIntensity(lightOn ? lightIntensity : 0.f);
	lampLight->SetLightColor(FLinearColor(1.f, 0.173f, 0.f));

	// Turn light on/off timeline
	TL_TurnLighOn = CreateDefaultSubobject<UTimelineComponent>(TEXT("Turn Light On"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Limit camera pitch
	auto cameraManager{ GetWorld()->GetFirstPlayerController()->PlayerCameraManager };
	cameraManager->ViewPitchMin = -35.f;
	cameraManager->ViewPitchMax = 40.f;

	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("SetLightIntensityFactor") });
	TL_TurnLighOn->AddInterpFloat(LightIntensityCurve, fcallback);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyCameraShake();
	UpdateStamina(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Moving
		if(MoveAction)
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		if(LookAction)
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// Turn on/off light
		if(LightAction)
			EnhancedInputComponent->BindAction(LightAction, ETriggerEvent::Started, this, &APlayerCharacter::LightLamp);

		// Running
		if (RunAction) {
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::StartSprint);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
		}
	}

}

void APlayerCharacter::Move(const FInputActionValue& Value) {
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value) {
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::LightLamp(const FInputActionValue& Value) {
	lightOn = not lightOn;

	if (lightOn)	TL_TurnLighOn->Play();
	else			TL_TurnLighOn->Reverse();
}

void APlayerCharacter::SetLightIntensityFactor(float intensityFactor) {
	lampLight->SetIntensity(lightIntensity * intensityFactor);
}

void APlayerCharacter::ApplyCameraShake() {
	auto* playerController{ GetWorld()->GetFirstPlayerController() };
	if (not playerController) return;

	auto velocity = GetVelocity().Length();

	if (velocity < WALK_SPEED) {
		if (StaticShake) playerController->ClientStartCameraShake(StaticShake);
	}
	else {
		if (MovementShake) playerController->ClientStartCameraShake(MovementShake, velocity/RUN_SPEED);
	}
}

bool APlayerCharacter::getLampState() const noexcept
{
	return lightOn;
}

void APlayerCharacter::StartSprint(const FInputActionValue& Value = {}) {
	if (exhausted || running) return;

	running = true;
	GetCharacterMovement()->MaxWalkSpeed = RUN_SPEED;
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value = {}) {
	running = false;
	GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
}

void APlayerCharacter::UpdateStamina(float time) {
	if (running) {
		// Loose stamina
		stamina -= time;

		if (stamina < 0.f) {
			StopSprint();
			exhausted = true;
		}
	}
	else if (stamina < MAX_STAMINA){
		// Recover stamina
		stamina = FMath::Min(stamina + time * STAMINA_RECOVER_RATE, MAX_STAMINA);
		
		if (exhausted && stamina > MIN_RUN_STAMINA) exhausted = false;
	}
}



void APlayerCharacter::setupStimulusSource()
{
	stimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));

	if (stimulusSource) {
		stimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		stimulusSource->RegisterWithPerceptionSystem();
	}
}