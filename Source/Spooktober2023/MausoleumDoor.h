// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MausoleumDoor.generated.h"

class UTextRenderComponent;
class UTimelineComponent;
class UCurveFloat;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmblemRequiredDelegate);

UCLASS()
class SPOOKTOBER2023_API AMausoleumDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMausoleumDoor();

	UFUNCTION()
	void InteractDoor(UActorComponent* component);

	UFUNCTION(BlueprintCallable)
	void PlaceEmblem(bool first);

	UFUNCTION(BlueprintCallable)
	void UpdateEmblemLocation(float factor);

	UFUNCTION(BlueprintCallable)
	void EmblemPlacedAnimationFinished();

	UFUNCTION(BlueprintCallable)
	void UpdateDoorsRotation(float factor);

	UFUNCTION(BlueprintCallable)
	void SetMausoleumName(const FName& newName);

	UFUNCTION(BlueprintCallable)
	void EmblemFound(int id);

	UFUNCTION(BlueprintCallable)
	void selectEmblemMaterial(int index);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Class variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName mausoleumName			{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int emblemId				{ -1 };
	int emblemsCollected		{ 0 };
	bool firstEmblemPlaced		{ false };
	bool secondEmblemPlaced		{ false };
	bool doorOpened				{ false };
	USceneComponent* selectedEmblem	{ nullptr };


	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* doorMesh{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* secondDoorMesh{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* emblemMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* secondEmblemMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_PlaceEmblem{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTimelineComponent* TL_OpenDoor{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* mausoleumFamiliyText{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* doorSound{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* DoorOpenCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* PlaceEmblemCurve{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	TArray<UMaterialInstance*> emblemsMaterial{ nullptr };

	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FEmblemRequiredDelegate OnEmblemRequired;

	constexpr int GetMousoleumEmblemId() { return emblemId; };
};
