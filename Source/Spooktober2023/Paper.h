// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Paper.generated.h"

UCLASS()
class SPOOKTOBER2023_API APaper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaper();

	UFUNCTION(BlueprintCallable)
	void SetPaperMessage(FText text);
	UFUNCTION(BlueprintCallable)
	void SetPaperTitle(FName t);
	UFUNCTION(BlueprintCallable)
	FText const GetPaperMessage();
	UFUNCTION(BlueprintCallable)
	FName const GetPaperTitle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Class variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paper")
	FText message{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paper")
	FName title{};

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* paperMesh{ nullptr };
};
