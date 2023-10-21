// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "Combat_Interface.h"
#include "Ghost.generated.h"


//Forward declarations
class USphereComponent;
class UBoxComponent;
class AAIController;
class UTimelineComponent;
/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API AGhost : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* sphereCollider{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* damageCollider{};

	AAIController* aiController{};


	UFUNCTION(BlueprintCallable)
	void setMaterialOpacity(float op);

public:
	//Class Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	UStaticMeshComponent* meshComp{ nullptr };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float minSightRadius{ 800.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float maxSightRadius{ 2000.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float minSpeed{ 10.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float maxSpeed{ 700.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float wanderSpeed{ 200.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float sphereRadius{ minSightRadius };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anger Level", meta = (AllowPrivateAccess = "true"))
	float speed{ minSpeed };


	bool teleportActorAI{ false };

	//Animations
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_Opacity{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* OpacityCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	UMaterialInterface* materialGhost{nullptr};

	UMaterialInstanceDynamic* dynamicMaterial{nullptr};
	// Sets default values for this character's properties
	AGhost();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	void PostInitializeComponents();

	// Called to bind functionality to input
	UBehaviorTree* GetBehaviorTree() const noexcept;

	void setSphereCollisionRadius(float radius);
	void setSpeedGhost(float speed);

	UTimelineComponent* getTimelineAnimation();

	

	UFUNCTION()
	void OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
	void OnAttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAttackCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* bTree{};
};
