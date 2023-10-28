// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ActivateCollisionBM.generated.h"

/**
 * 
 */
UCLASS()
class SPOOKTOBER2023_API UAnimNotify_ActivateCollisionBM : public UAnimNotify
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material")
	bool animationState{ false };

	virtual void Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
	
};
