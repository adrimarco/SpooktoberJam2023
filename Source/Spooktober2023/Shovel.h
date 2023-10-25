// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shovel.generated.h"

class UTimelineComponent;
class UCurveVector;

UCLASS()
class SPOOKTOBER2023_API AShovel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShovel();

	UFUNCTION(BlueprintCallable)
	void SetShovelPosition(const FVector& pos);

	UFUNCTION(BlueprintCallable)
	void SetShovelRotation(const FVector& rot);

	UFUNCTION(BlueprintCallable)
	void PlayAnimation();

	UFUNCTION(BlueprintCallable)
	void StopAnimation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* shovelMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_Dig{ nullptr };

	// Timeline curve
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveVector* PositionCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveVector* RotationCurve{ nullptr };
};
