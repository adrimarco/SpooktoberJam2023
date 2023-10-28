// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ActivateCollisionBM.h"
#include "Monster.h"

void UAnimNotify_ActivateCollisionBM::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr) {
		if (AMonster* monster = Cast<AMonster>(MeshComp->GetOwner())) {
			monster->activateAttackCollision(animationState);
		}

	}
}
