// Copyright Epic Games, Inc. All Rights Reserved.

#include "MySqlPluginGameMode.h"
#include "MySqlPluginCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMySqlPluginGameMode::AMySqlPluginGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
