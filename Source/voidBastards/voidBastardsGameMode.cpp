// Copyright Epic Games, Inc. All Rights Reserved.

#include "voidBastardsGameMode.h"
#include "voidBastardsHUD.h"
#include "voidBastardsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AvoidBastardsGameMode::AvoidBastardsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/testChar"));//FirstPersonCPP/Blueprints/FirstPersonCharacterr"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AvoidBastardsHUD::StaticClass();
}
