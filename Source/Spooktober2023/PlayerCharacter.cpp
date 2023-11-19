// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SphereComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Monster.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// Interaction actors
#include "Coffin.h"
#include "Grave.h"
#include "Paper.h"
#include "MausoleumDoor.h"
#include "Shovel.h"


constexpr auto MAX_HEALTH				= 3;
constexpr auto CHARM_COOLDOWN_TIME		= 40.f;
constexpr auto LIGHT_INTENSITY			= 5000.f;
constexpr auto MIN_LIGHT_INTENSITY		= 500.f;
constexpr auto LIGHT_ATTENUATION_RADIUS = 4000.f;
constexpr auto MIN_ATTENUATION_RADIUS	= 2000.f;
constexpr auto MAX_STAMINA				= 10.f;
constexpr auto MIN_RUN_STAMINA			= 5.f;
constexpr auto STAMINA_RECOVER_RATE		= 0.8f;
constexpr auto WALK_SPEED				= 400.f;
constexpr auto RUN_SPEED				= 800.f;
constexpr auto INTERACTION_DISTANCE		= 300.f;
constexpr auto INTERACTING_CAMERA_SLOW	= 0.15f;
constexpr auto INTERACTING_CAM_MOV_LIMIT= 0.4f;
const FVector CHARM_INIT_LOCATION		= FVector{35.f, 20.f, -32.f};
const FVector CHARM_FINAL_LOCATION		= FVector{13.f, 0.f, 0.f};
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
	camera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
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

	//NavMesh modifier collision
	navmeshModifier = CreateDefaultSubobject<USphereComponent>(TEXT("Navigation Modifier"));
	navmeshModifier->SetupAttachment(GetCapsuleComponent());
	navmeshModifier->SetSphereRadius(200.f);
	navmeshModifier->Deactivate();
	
	// Lamp mesh
	lampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lamp"));
	lampMesh->SetupAttachment(GetCapsuleComponent());
	lampMesh->SetRelativeLocation(FVector(25.f, -18.f, 40.f));
	lampMesh->SetRelativeRotation(FRotator(-3.5f, 70.f, -9.7f));

	// Lamp light
	lampLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Lamp Point Light"));
	lampLight->SetupAttachment(lampMesh);
	lampLight->SetAttenuationRadius(LIGHT_ATTENUATION_RADIUS);
	lightIntensity = LIGHT_INTENSITY;
	lampLight->SetLightColor(FLinearColor(1.f, 0.173f, 0.f));
	lampLight->SetRelativeLocation(FVector(0.f, -1.17f, 10.f));

	//Lamp particle
	niagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Lamp Particles"));
	//niagaraComp->SetNiagaraVariableFloat(FString("Size"), fireSize);
	niagaraComp->SetupAttachment(lampMesh);
	niagaraComp->SetRelativeLocation(FVector(0.f, -0.9f, 9.f));

	// Charm mesh
	charmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Charm Mesh"));
	charmMesh->SetupAttachment(camera);
	charmMesh->SetRelativeLocation(CHARM_INIT_LOCATION);
	charmMesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	// Audio components
	stepsSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Steps Sound"));
	stepsSound->SetupAttachment(GetRootComponent());
	stepsSound->bAutoActivate = false;
	stepsSound->bOverrideAttenuation = true;
	stepsSound->bAllowSpatialization = true;

	lampSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Lamp Sound"));
	lampSound->SetupAttachment(GetRootComponent());
	lampSound->bAutoActivate = false;

	// Child actors
	shovelActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Shovel Actor"));
	shovelActor->SetupAttachment(GetRootComponent());

	// Turn light on/off timeline
	TL_TurnLighOn = CreateDefaultSubobject<UTimelineComponent>(TEXT("Turn Light On"));

	//Dead timeline
	TL_Dead = CreateDefaultSubobject<UTimelineComponent>(TEXT("Player Dead Animation"));

	// Charm timeline
	TL_FocusCharm = CreateDefaultSubobject<UTimelineComponent>(TEXT("Focus Charm Animation"));
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
	lampLight->SetIntensity(lightOn ? lightIntensity : MIN_LIGHT_INTENSITY);
	lampLight->SetAttenuationRadius(lightOn ? LIGHT_ATTENUATION_RADIUS : MIN_ATTENUATION_RADIUS);

	HideCharm();

	// Limit camera pitch
	auto cameraManager{ GetWorld()->GetFirstPlayerController()->PlayerCameraManager };
	cameraManager->ViewPitchMin = -45.f;
	cameraManager->ViewPitchMax = 40.f;

	// Configure steps sound
	stepsSound->SetIntParameter("Speed", 0);
	stepsSound->SetIntParameter("FloorType", 0);

	//Configure fire particles
	niagaraComp->SetNiagaraVariableFloat(FString("Size"), 0.1);

	// Child actors
	if (shovelBP != nullptr) shovelActor->SetChildActorClass(shovelBP);

	// Timeline binding functions
	FOnTimelineFloat fcallback;
	fcallback.BindUFunction(this, FName{ TEXT("SetLightIntensityFactor") });
	TL_TurnLighOn->AddInterpFloat(LightIntensityCurve, fcallback);

	FOnTimelineVector vcallback;
	vcallback.BindUFunction(this, FName{ TEXT("setDeadAnimation") });
	TL_Dead->AddInterpVector(DeadAnimationCurve, vcallback);
	
	fcallback.Unbind();
	fcallback.BindUFunction(this, FName{ TEXT("UpdateRotationWhileFocusCharm") });
	TL_FocusCharm->AddInterpFloat(FocusCharmCurve, fcallback);

	fcallback.Unbind();
	fcallback.BindUFunction(this, FName{ TEXT("SetCameraFOV") });
	TL_FocusCharm->AddInterpFloat(FocusCharmCameraFovCurve, fcallback);

	fcallback.Unbind();
	fcallback.BindUFunction(this, FName{ TEXT("SetCharmMeshPosition") });
	TL_FocusCharm->AddInterpFloat(FocusCharmZPositionCurve, fcallback);

	// Bind timeline finish functions
	FOnTimelineEventStatic finishCallback;
	finishCallback.BindUFunction(this, FName{ TEXT("endDeadAnimation") });
	TL_Dead->SetTimelineFinishedFunc(finishCallback);

	finishCallback.Unbind();
	finishCallback.BindUFunction(this, FName{ TEXT("EndCharmEffect") });
	TL_FocusCharm->SetTimelineFinishedFunc(finishCallback);

	// Initial papers
	collectedPapers.Empty();
	for (auto i{ 0 }; i < initialPapers.Num(); ++i) {
		AddPaperOrdered(initialPapers[i]);
	}

	// Set player initial speed
	GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;
	stamina = MAX_STAMINA;

	// Initialize player health and other variables
	health = MAX_HEALTH;
	goalCompleted = false;
	uiWidgetActive = false;
	charmCooldown = 999.f;
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	navmeshModifier->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCollisionBeginOverlap);
	navmeshModifier->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnCollisionEndOverlap);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyCameraShake();
	UpdateStamina(DeltaTime);
	UpdateCharm(DeltaTime);

	if (GetVelocity().IsZero() && GetWorldTimerManager().IsTimerActive(stepsSoundHandle)) GetWorldTimerManager().ClearTimer(stepsSoundHandle);
	else CheckFloorMaterial();
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

		// Charm
		if (CharmAction) {
			EnhancedInputComponent->BindAction(CharmAction, ETriggerEvent::Triggered, this, &APlayerCharacter::RequestCharmEffect);
			EnhancedInputComponent->BindAction(CharmAction, ETriggerEvent::Ongoing, this, &APlayerCharacter::RiseCharm);
			EnhancedInputComponent->BindAction(CharmAction, ETriggerEvent::Canceled, this, &APlayerCharacter::HideCharm);
		}
	}

}

void APlayerCharacter::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMonster* mons = Cast<AMonster>(OtherActor)) {
		if (lightOn) {
			AAIController* aiCont = Cast<AAIController>(mons->GetController());
			aiCont->GetBlackboardComponent()->SetValueAsBool("attackSuccess", true);
		}
	}
}

void APlayerCharacter::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void APlayerCharacter::Move(const FInputActionValue& Value) {
	// Check player has a controller or movement is disabled
	if (Controller == nullptr || blockInput || blockMovement) return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Add movement 
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);

	// Steps sound
	if (not stepsSound->IsPlaying()) UpdateStepsSound();
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
	if (blockInput || blockLamp || TL_TurnLighOn->IsPlaying()) return;

	lightOn = not lightOn;

	// Control area that protects player from monster
	if (lightOn)	navmeshModifier->Activate();
	else			navmeshModifier->Deactivate();
	
	// Play sound
	lampSound->SetBoolParameter("TurnOn", true);
	lampSound->Play();

	// Play animation
	if (lightOn) { 
		TL_TurnLighOn->PlayFromStart(); 
		niagaraComp->SetNiagaraVariableFloat(FString("Size"), 0.2); 
	}
	else { 
		TL_TurnLighOn->ReverseFromEnd(); 
		niagaraComp->SetNiagaraVariableFloat(FString("Size"), 0.1);
	}
}

void APlayerCharacter::extinguishLamp(float time)
{
	// Turn off lamp
	lightOn = false;

	navmeshModifier->Deactivate();

	if (TL_TurnLighOn->IsPlaying()) TL_TurnLighOn->Stop();
	TL_TurnLighOn->SetPlaybackPosition(0.f, false);
	lampLight->SetIntensity(0.f);

	// Play sound
	lampSound->SetBoolParameter("TurnOn", false);
	lampSound->Play();


	niagaraComp->SetNiagaraVariableFloat(FString("Size"), 0.0);

	// Prevent lighting it again
	blockLamp = true;

	// If light was already extinguished, clears timer
	auto& timerManager{ GetWorldTimerManager() };
	if (timerManager.IsTimerActive(extinguishedLightHandle))
		timerManager.ClearTimer(extinguishedLightHandle);

	// Set timer for recovering light
	timerManager.SetTimer(extinguishedLightHandle, this, &APlayerCharacter::igniteLamp, time, false, time);
}

void APlayerCharacter::igniteLamp() {
	// Recover lamp light
	blockLamp = false;
	lightOn = false;
	lampLight->SetIntensity(MIN_LIGHT_INTENSITY);
	lampLight->SetAttenuationRadius(MIN_ATTENUATION_RADIUS);
	niagaraComp->SetNiagaraVariableFloat(FString("Size"), 0.1);
}

void APlayerCharacter::SetLightIntensityFactor(float intensityFactor) {
	lampLight->SetIntensity(MIN_LIGHT_INTENSITY + (lightIntensity - MIN_LIGHT_INTENSITY) * intensityFactor);
	lampLight->SetAttenuationRadius(MIN_ATTENUATION_RADIUS + (LIGHT_ATTENUATION_RADIUS - MIN_ATTENUATION_RADIUS) * intensityFactor);
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

	UpdateStepsSound();
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value = {}) {
	running = false;
	GetCharacterMovement()->MaxWalkSpeed = WALK_SPEED;

	UpdateStepsSound();
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

void APlayerCharacter::UpdateCharm(float time) {
	if (charmCooldown > 0.f) charmCooldown -= time;
}

bool APlayerCharacter::IsCharmAvailable() {
	return charmCooldown <= 0.f && not uiWidgetActive;
}

int32 APlayerCharacter::AddPaperOrdered(const PaperMessage& paper) {
	for (int32 i{ 0 }; i < collectedPapers.Num(); ++i) {
		if (paper.title.Compare(collectedPapers[i].title) < 0) {
			return collectedPapers.Insert(paper, i);
		}
	}

	return collectedPapers.Add(paper);;
}

void APlayerCharacter::setDeadAnimation(const FVector& rot)
{
	camera->SetRelativeRotation(FRotator(rot.X, rot.Y, rot.Z));
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

				if (graveActor->StartDigging(this)) {
					slowCamera = true;
					blockMovement = true;

					shovelActor->SetAbsolute(true);
					shovelActor->SetWorldLocation(raycastResult.ImpactPoint);
					Cast<AShovel>(shovelActor->GetChildActor())->PlayAnimation();
				}

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

void APlayerCharacter::RequestCharmEffect(const FInputActionValue& Value = {}) {
	// Check charm is not on cooldown and player is not using inventory
	if (not IsCharmAvailable()) return;

	OnCharm.Broadcast();
}

void APlayerCharacter::RiseCharm(const FInputActionInstance& Value) {
	// Check charm is available
	if (not IsCharmAvailable()) return;

	// Check charm is not being used
	if (Value.GetTriggeredTime() > 0.f) return;

	auto triggers{ Value.GetTriggers() };

	for (auto i{ 0 }; i < triggers.Num(); ++i) {
		if (triggers[i]->IsA<UInputTriggerHold>()) {
			float totalHoldTime{ Cast<UInputTriggerHold>(Value.GetTriggers()[i])->HoldTimeThreshold };
			float currentHoldTime{ Value.GetElapsedTime() };

			// Update charm position
			SetCharmMeshPosition(currentHoldTime / totalHoldTime);

			break;
		}
	}
}

void APlayerCharacter::HideCharm(const FInputActionInstance&) {
	SetCharmMeshPosition(-1.f);
}

void APlayerCharacter::StopInteract(const FInputActionValue& Value = {}) {
	if (interactingWith == nullptr) return;

	if (interactingWith->IsA<AGrave>()) {
		AGrave* graveActor{ Cast<AGrave>(interactingWith) };

		graveActor->StopDigging();
		Cast<AShovel>(shovelActor->GetChildActor())->StopAnimation();
		shovelActor->SetAbsolute(false);
		shovelActor->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
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
		UGameplayStatics::SetGamePaused(GetWorld(), false);
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

void APlayerCharacter::StopDigging() {
	if (interactingWith == nullptr || not interactingWith->IsA<AGrave>()) return;

	StopInteract();
}

void APlayerCharacter::GraveAreaEntered(AGrave* grave) {
	OnGraveAreaEntered.Broadcast(grave);
}


void APlayerCharacter::enterSecureZone(bool enterArea)
{
	OnEnterSecureArea.Broadcast(enterArea);
}

void APlayerCharacter::decreaseHealth(int damage)
{
	// Reduce life
	health -= damage;

	// If was using charm, stop animation
	if (TL_FocusCharm->IsPlaying()) {
		TL_FocusCharm->Stop();
		EndCharmEffect();
	}

	// Notify damage
	OnPlayerDamaged.Broadcast(health);

	// Check if player still has lives
	if (health <= 0) {
		// Player dies
		camera->bUsePawnControlRotation = false;
		TL_Dead->Play();
		blockInput = true;
	}
	else if (blockLamp || lightOn) {
		// Restore extinguished lamp when being damaged
		blockLamp = false;

		auto& timerManager{ GetWorldTimerManager() };
		if (timerManager.IsTimerActive(extinguishedLightHandle))
			timerManager.ClearTimer(extinguishedLightHandle);

		igniteLamp();
	}
}

void APlayerCharacter::endDeadAnimation()
{
	camera->bUsePawnControlRotation = true;
	OnEndDeadAnimation.Broadcast();
}

void APlayerCharacter::UseCharm(bool objectiveFound, FVector location) {
	// Check can use charm
	if (not IsCharmAvailable() || not objectiveFound) return;

	// Calculate objetive rotation
	objectiveRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), location);

	// Block input and play animation
	blockInput = true;
	TL_FocusCharm->PlayFromStart();

	// Starts cooldown
	charmCooldown = CHARM_COOLDOWN_TIME;
}

void APlayerCharacter::EndCharmEffect() {
	// Enable input again
	blockInput = false;
}

void APlayerCharacter::ResetCharmCooldown() {
	charmCooldown = 0.f;
}

void APlayerCharacter::SetCharmMeshPosition(float factor) {
	if (factor <= 0.f) {
		// Hides charm
		charmMesh->SetVisibility(false);
		return;
	}

	// Update position
	//auto location{ charmMesh->GetRelativeLocation() };
	//location.Z = FMath::Lerp(CHARM_INIT_Z, CHARM_FINAL_Z, FMath::Clamp(factor, 0.f, 1.f));
	auto location{ FMath::Lerp(CHARM_INIT_LOCATION, CHARM_FINAL_LOCATION, FMath::Clamp(factor, 0.f, 1.f)) };

	charmMesh->SetRelativeLocation(location);

	// Show charm
	charmMesh->SetVisibility(true);
}

void APlayerCharacter::UpdateRotationWhileFocusCharm(float factor) {
	factor = FMath::Clamp(factor, 0.f, 1.f);

	// Interpolate rotation
	FRotator newRotation{ FMath::Lerp(GetActorRotation(), objectiveRotation, factor)};
	
	GetController()->SetControlRotation(newRotation);
}

void APlayerCharacter::SetCameraFOV(const float& fov) {
	camera->SetFieldOfView(FMath::Clamp(fov, 5.f, 300.f));
}

void APlayerCharacter::exitToMenu()
{
	OnPlayerDead.Broadcast();
}

void APlayerCharacter::UpdateStepsSound() {
	float playRate{ 0.7f };
	FTimerManager& tm = GetWorldTimerManager();

	if (running) playRate = 0.4f;

	// Check if exists other timer
	if (tm.IsTimerActive(stepsSoundHandle) && tm.GetTimerRate(stepsSoundHandle) != playRate) {
		tm.ClearTimer(stepsSoundHandle);
		tm.SetTimer(stepsSoundHandle, this, &APlayerCharacter::PlayStepsSound, playRate, true, 0);
	}
	else if (not GetVelocity().IsZero() && not tm.IsTimerActive(stepsSoundHandle)) {
		tm.SetTimer(stepsSoundHandle, this, &APlayerCharacter::PlayStepsSound, playRate, true, 0);
	}
}

void APlayerCharacter::CheckFloorMaterial() {
	int material{ 0 };

	// Raycast to get actor under player
	FVector from{ GetActorLocation()};
	FVector to{ from - FVector(0.f, 0.f, 100.f)};

	FHitResult raycastResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(raycastResult, from, to, ECC_Visibility, params)) {
		// Check if actor has a footsteps sound tag
		AActor* actor{ raycastResult.GetActor() };
		if		(actor->ActorHasTag("WoodenFloor"))		material = 1;
		else if (actor->ActorHasTag("ConcreteFloor"))	material = 2;
		else											material = 0;
	}
	else {
		material = 0;
	}

	// Update sound parameter
	stepsSound->SetIntParameter("FloorType", material);
}

void APlayerCharacter::PlayStepsSound() {
	stepsSound->Play();
}