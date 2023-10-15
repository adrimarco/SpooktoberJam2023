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
#include "Blueprint/UserWidget.h"
// Interaction actors
#include "Coffin.h"
#include "Grave.h"
#include "Paper.h"


constexpr auto LIGHT_INTENSITY			= 5000.f;
constexpr auto LIGHT_ATTENUATION_RADIUS = 2000.f;
constexpr auto MAX_STAMINA				= 10.f;
constexpr auto MIN_RUN_STAMINA			= 5.f;
constexpr auto STAMINA_RECOVER_RATE		= 0.8f;
constexpr auto WALK_SPEED				= 400.f;
constexpr auto RUN_SPEED				= 800.f;
constexpr auto INTERACTION_DISTANCE		= 300.f;
constexpr auto INTERACTING_CAMERA_SLOW	= 0.15f;
constexpr auto INTERACTING_CAM_MOV_LIMIT= 0.4f;


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
	postProcessSettings.MotionBlurAmount = 0.2;
	postProcessSettings.MotionBlurMax = 20;
	postProcessSettings.bOverride_BloomMethod = true;
	postProcessSettings.BloomMethod = EBloomMethod::BM_SOG;

	camera->PostProcessSettings = postProcessSettings;

	// Lamp mesh
	lampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lamp"));
	lampMesh->SetupAttachment(GetCapsuleComponent());
	lampMesh->SetRelativeLocation(FVector(23.8f, -18.f, 30.f));
	lampMesh->SetRelativeRotation(FRotator(-3.5f, 70.f, -9.7f));

	// Lamp light
	lampLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Lamp Point Light"));
	lampLight->SetupAttachment(lampMesh);
	lampLight->SetAttenuationRadius(LIGHT_ATTENUATION_RADIUS);
	lightIntensity = LIGHT_INTENSITY;
	lampLight->SetLightColor(FLinearColor(1.f, 0.173f, 0.f));
	lampLight->SetRelativeLocation(FVector(0.f, -1.17f, 10.f));

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

	// Initial light state
	lampLight->SetIntensity(lightOn ? lightIntensity : 0.f);

	// Limit camera pitch
	auto cameraManager{ GetWorld()->GetFirstPlayerController()->PlayerCameraManager };
	cameraManager->ViewPitchMin = -45.f;
	cameraManager->ViewPitchMax = 40.f;

	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("SetLightIntensityFactor") });
	TL_TurnLighOn->AddInterpFloat(LightIntensityCurve, fcallback);

	GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
	stamina = MAX_STAMINA;
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

		// Interact
		if (InteractAction) {
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopInteract);
		}

		// Back
		if (BackAction) 
			EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ClosePaper);
	}

}

void APlayerCharacter::Move(const FInputActionValue& Value) {
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Check player has a controller or movement is disabled
	if (Controller == nullptr || blockMovement) return;

	// Add movement 
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value) {
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// Check player has a controller or camera movement is disabled
	if (Controller == nullptr || blockCamera) return;

	// Reduces camera speed
	if (slowCamera) LookAxisVector *= INTERACTING_CAMERA_SLOW;

	// Add yaw and pitch input to controller
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

	if (interactingWith != nullptr) {
		FVector newRotation = camera->GetForwardVector();
		
		if (FVector::DotProduct(interactionDirection, newRotation) < INTERACTING_CAM_MOV_LIMIT) {
			StopInteract({});
		}
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
	if (running && not GetCharacterMovement()->Velocity.IsZero()) {
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

void APlayerCharacter::Interact(const FInputActionValue& Value) {
	// Raycast to get actor in sight
	FVector from	{ camera->GetComponentLocation() };
	FVector to		{ from + camera->GetComponentRotation().Vector() * INTERACTION_DISTANCE };

	FHitResult raycastResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(raycastResult, from, to, ECC_PhysicsBody, params)) {
		AActor* hitActor = raycastResult.GetActor();
		if (hitActor->IsChildActor() && not hitActor->ActorHasTag("Interactable")) hitActor = hitActor->GetParentActor();

		if (hitActor->ActorHasTag("Interactable") && not raycastResult.GetComponent()->ComponentHasTag("BlockInteraction")) {
			// Check if hit actor is a coffin
			if (hitActor->IsA<ACoffin>()) {
				ACoffin* coffinActor{ Cast<ACoffin>(hitActor) };

				coffinActor->OpenCoffin(this);
				interactingWith = hitActor;
			}
			// Check if hit actor is a grave
			else if (hitActor->IsA<AGrave>()) {
				AGrave* graveActor{ Cast<AGrave>(hitActor) };

				graveActor->StartDigging();
				slowCamera = true;
				blockMovement = true;

				interactingWith = hitActor;
			}
			// Check if hit actor is a paper
			else if (hitActor->IsA<APaper>()) {
				APaper* paperActor{ Cast<APaper>(hitActor) };

				auto index = collectedPapers.Add({ paperActor->message, paperActor->title });

				// Show paper
				OnPaperCollected.Broadcast(collectedPapers[index].message);
				blockCamera = true;
				blockMovement = true;

				interactingWith = hitActor;
			}

			interactionDirection = camera->GetForwardVector();
		}
	}
	else {
		interactingWith = nullptr;
	}
}

void APlayerCharacter::StopInteract(const FInputActionValue& Value = {}) {
	if (interactingWith == nullptr) return;

	if (interactingWith->IsA<AGrave>()) {
		AGrave* graveActor{ Cast<AGrave>(interactingWith) };

		graveActor->StopDigging();
		slowCamera = false;
		blockMovement = false;
	}
	else if (interactingWith->IsA<APaper>()) {
		APaper* paperActor{ Cast<APaper>(interactingWith) };

		paperActor->Destroy();
	}

	interactingWith = nullptr;
}

void APlayerCharacter::SetMoney(int m = 0){
	money = m;
}


void APlayerCharacter::AddMoney(int m) {
	money += m;
}

void APlayerCharacter::ClosePaper() {
	if (paperWidget != nullptr) {
		paperWidget->RemoveFromParent();

		blockCamera = false;
		blockMovement = false;

		paperWidget = nullptr;
	}
}
