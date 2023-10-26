// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "Combat_Interface.h"
#include "Monster.generated.h"

class UStaticMeshComponent;

UCLASS()
class SPOOKTOBER2023_API AMonster : public ACharacter, public ICombat_Interface
{
	GENERATED_BODY()

public:
	//Class Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	UStaticMeshComponent* meshComp{nullptr};

	// Sets default values for this character's properties
	AMonster();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	UBehaviorTree* GetBehaviorTree() const noexcept;

	int MeleeAttack_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void playerEnteredSecureArea(bool playerEntered);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(AllowPrivateAccess="true"))
	UBehaviorTree* bTree{};
};
