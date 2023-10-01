// Copyright Epic Games, Inc. All Rights Reserved.

#include "Spooktober2023GameMode.h"
#include "Spooktober2023Character.h"
#include "UObject/ConstructorHelpers.h"

ASpooktober2023GameMode::ASpooktober2023GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
