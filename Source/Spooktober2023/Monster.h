// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "Combat_Interface.h"
#include "Monster.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;
class UAudioComponent;
class UBoxComponent;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTouchLightDelegate);

UCLASS()
class SPOOKTOBER2023_API AMonster : public ACharacter, public ICombat_Interface
{
	GENERATED_BODY()

public:
	//Class Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	UStaticMeshComponent* meshComp{nullptr};


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpotLightComponent* lampLight{ nullptr };

	bool CanHitPlayer{ true };

	// Sets default values for this character's properties
	AMonster();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PostInitializeComponents();

	// Called to bind functionality to input
	UBehaviorTree* GetBehaviorTree() const noexcept;

	int MeleeAttack_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void playerEnteredSecureArea(bool playerEntered);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* idleSound{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* attackSound{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* attackCollider{};

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FTouchLightDelegate OnTouchLight;

	UFUNCTION()
	void activateAttackCollision(bool activate);

	UFUNCTION()
	void OnAttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAttackCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(AllowPrivateAccess="true"))
	UBehaviorTree* bTree{};
};
