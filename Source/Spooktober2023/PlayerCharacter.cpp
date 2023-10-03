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


#define LIGHT_INTENSITY				5000.f
#define LIGHT_ATTENUATION_RADIUS	2000.f


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	postProcessSettings.MotionBlurAmount = 0.5;
	postProcessSettings.MotionBlurMax = 50;

	camera->PostProcessSettings = postProcessSettings;

	// Lamp mesh
	lampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lamp"));
	lampMesh->SetupAttachment(camera);

	// Lamp light
	lampLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Lamp Point Light"));
	lampLight->SetupAttachment(lampMesh);
	lampLight->SetAttenuationRadius(LIGHT_ATTENUATION_RADIUS);
	lightIntensity = LIGHT_INTENSITY;
	lampLight->SetIntensity(lightIntensity);
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

	if (GetVelocity().Length() < 1.f) {
		if (StaticShake) playerController->ClientStartCameraShake(StaticShake);
	}
	else {
		if (MovementShake) playerController->ClientStartCameraShake(MovementShake);
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