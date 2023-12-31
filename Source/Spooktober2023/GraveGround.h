// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GraveGround.generated.h"

UCLASS()
class SPOOKTOBER2023_API AGraveGround : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGraveGround();

	UFUNCTION(BlueprintCallable)
	void SetGroundLocation(FVector location);
	UFUNCTION(BlueprintCallable)
	void SetGroundScale(float scale);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TArray<UStaticMeshComponent*> groundMeshes{ nullptr };
};
