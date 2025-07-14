// Fill out your copyright notice in the Description page of Project Settings.


#include "LGGameMode.h"

#include "LGHUD.h"
#include "LegoGame/LegoGame.h"
#include "Player/LGPlayerCharacter.h"
#include "Player/LGPlayerController.h"
ALGGameMode::ALGGameMode()
{
	ConstructorHelpers::FClassFinder<ALGPlayerCharacter> PlayerCharacterClass(TEXT("Blueprint'/Game/Lego/BP/Player/BP_Player.BP_Player_C'"));
	DefaultPawnClass=PlayerCharacterClass.Class;

	PlayerControllerClass=ALGPlayerController::StaticClass();
	HUDClass=ALGHUD::StaticClass();
}

void ALGGameMode::BeginPlay()
{
	Super::BeginPlay();
	FGenericTeamId::SetAttitudeSolver([](FGenericTeamId A,FGenericTeamId B)
	{
		if(A==TeamID_Yellow || B==TeamID_Yellow)
		{
			return ETeamAttitude::Neutral;
		}
		return A!=B?ETeamAttitude::Hostile:ETeamAttitude::Friendly;
	});
}
