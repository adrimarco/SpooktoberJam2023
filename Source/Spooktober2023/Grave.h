// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grave.generated.h"

class AGraveGround;
class ACoffin;
class UTimelineComponent;
class UCurveFloat;
class UCurveVector;
class UBoxComponent;

UCLASS()
class SPOOKTOBER2023_API AGrave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrave();

	UFUNCTION(BlueprintCallable)
	void SetCoffinHeight(float height);

	UFUNCTION(BlueprintCallable)
	void StartDigging();

	UFUNCTION(BlueprintCallable)
	void StopDigging();

	UFUNCTION(BlueprintCallable)
	void FinishDigging();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Class variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dead Information")
	FName deadName{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dead Information")
	int tombValue{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grave State")
	bool digged{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grave State")
	bool looted{ false };

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* graveMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* coffinActor{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* graveGroundActor{ nullptr };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_Dig{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* boxCollision{ nullptr };
	
	// Timeline curves
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveVector* GroundLocationCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* GroundScaleCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* CoffinPositionCurve{ nullptr };
	
	// Blueprint classes for child actors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components Blueprint")
	TSubclassOf<AGraveGround> groundBP{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components Blueprint")
	TSubclassOf<ACoffin> coffinBP{ nullptr };
};
