// Copyright Epic Games, Inc. All Rights Reserved.

#include "Colorful_Architect3DGameMode.h"
#include "Colorful_Architect3DCharacter.h"
#include "UObject/ConstructorHelpers.h"

AColorful_Architect3DGameMode::AColorful_Architect3DGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
