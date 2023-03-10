// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASystemGameMode.h"
#include "GASystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGASystemGameMode::AGASystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
