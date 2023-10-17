// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coffin.generated.h"

class UTimelineComponent;
class APlayerCharacter;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBadCoffinOppened, AActor*, coffinActor);

UCLASS()
class SPOOKTOBER2023_API ACoffin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoffin();

	UFUNCTION(BlueprintCallable)
	static void SetJewelsCollection(const TArray<UStaticMesh*>& col);

	UFUNCTION(BlueprintCallable)
	static void ClearJewelsCollection();

	UFUNCTION(BlueprintCallable)
	void SetOutcomeValue(bool negative);

	UFUNCTION(BlueprintCallable)
	void SetTombValue(int value);

	UFUNCTION(BlueprintCallable)
	void SetCanBeOpened(bool value);

	UFUNCTION(BlueprintCallable)
	void OpenCoffin(APlayerCharacter* player);

	UFUNCTION(BlueprintCallable)
	void SetCoffinDoorRotation(float rot);

	UFUNCTION(BlueprintCallable)
	void UpdateMeshes();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Class variables
	int tombValue{ 0 };
	bool negativeOutcome{ false };
	bool canBeOpened{ false };
	bool opened{ false };

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* coffinMesh{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* coffinDoorMesh{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* bodyMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* jewelMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_Open{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* DoorOpenCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	TArray<UStaticMesh*> jewels{ nullptr };

	// Event dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FBadCoffinOppened OnBadCoffinOppened;

	inline static TArray<UStaticMesh*> jewelsCollection{};
};
