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
#include "MausoleumDoor.h"


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
TArray<PaperMessage> initialPapers = {	{FText::FromString("So, are you determined to go to the cemetery? Is that the only thing you can think of? I don't intend to judge you; everyone does what they can to move forward. I'll just offer you a piece of advice. Meddling in the affairs of the dead can be dangerous. Disturbing troubled, malevolent, or unhappy souls comes with its risks. You should think carefully about whom you're looting, although it's impossible to do so without knowing who they were. I wish you the best of luck; you're going to need it."), 
										"Rest in peace?"},
										{FText::FromString("I don't know what reasons you have for venturing into the forest of the dead on the night before All Saints' Day, but I don't believe it's a sensible idea. I must warn you in case you're not aware of what you're about to do. They say that on October 31st, in the dead of night, the sky turns red, the souls of the departed wander our world once again, and their secrets come to light. I suppose you may not believe in such nonsense, but I'd carry a lantern. Some claim it can protect you from the spirits, although others insist just the opposite."),
										"The Red Night"}, 
										{FText::FromString("We've been quite patient, haven't we? This is your last chance. You have one week to gather all the money and hand it over to us. We don't care how you do it, even if you have to steal from all the elderly people in the city. If we haven't seen the money by November 1st, you'd better prepare for the worst."),
										"Final warning"}, 
};


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
	uiWidgetActive = false;

	// Initial papers
	collectedPapers.Empty();
	for (auto i{ 0 }; i < initialPapers.Num(); ++i) {
		AddPaperOrdered(initialPapers[i]);
	}
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
	// Check player has a controller or movement is disabled
	if (Controller == nullptr || blockInput || blockMovement) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Add movement 
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value) {
	// Check player has a controller or camera movement is disabled
	if (Controller == nullptr || blockInput || blockCamera) return;

	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

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

void APlayerCharacter::LightLamp(const FInputActionValue& Value = {}) {
	if (blockInput) return;

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

int32 APlayerCharacter::AddPaperOrdered(const PaperMessage& paper) {
	for (int32 i{ 0 }; i < collectedPapers.Num(); ++i) {
		if (paper.title.Compare(collectedPapers[i].title) < 0) {
			return collectedPapers.Insert(paper, i);
		}
	}

	return collectedPapers.Add(paper);;
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
	if (blockInput) return;

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

				auto index = AddPaperOrdered({ paperActor->message, paperActor->title });
				
				// Show paper
				OnPaperCollected.Broadcast(collectedPapers[index].message);
				blockInput = true;

				interactingWith = hitActor;
			}
			else if (hitActor->IsA<AMausoleumDoor>()) {
				AMausoleumDoor* doorActor{ Cast<AMausoleumDoor>(hitActor) };

				doorActor->InteractDoor(raycastResult.GetComponent());

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
	OnMoneyIncrease.Broadcast(m, money);
}

void APlayerCharacter::ClosePaper() {
	if (paperWidget != nullptr) {
		paperWidget->RemoveFromParent();

		if(not uiWidgetActive)
			blockInput = false;

		paperWidget = nullptr;
	}
}

TArray<FName> APlayerCharacter::getCollectedPapersTitles() const {
	TArray<FName> papers;

	for (auto i{ 0 }; i < collectedPapers.Num(); ++i) {
		papers.Add(collectedPapers[i].title);
	}

	return papers;
}

FText APlayerCharacter::getPaperText(const FName& title) const {
	for (auto i{ 0 }; i < collectedPapers.Num(); ++i) {
		if (collectedPapers[i].title.Compare(title) == 0) {
			return collectedPapers[i].message;
		}
	}

	return FText();
}

void APlayerCharacter::SetMausoleumNumber(int size) {
	collectedEmblems.SetNum(FMath::Max(size, 0));

	for (int i{ 0 }; i < collectedEmblems.Num(); ++i) {
		collectedEmblems[i] = 0;
	}
}

void APlayerCharacter::EmblemCollected(int id) {
	if (collectedEmblems.IsValidIndex(id)) {
		collectedEmblems[id]++;
	}
	OnEmblemCollected.Broadcast(id);
}

TArray<int> APlayerCharacter::GetCollectedEmblems() {
	return collectedEmblems;
}
