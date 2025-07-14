// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LGPlayerController.h"

#include "LegoGame/LegoGame.h"
#include "Manager/LGPlayerCameraManager.h"

ALGPlayerController::ALGPlayerController()
{
	PlayerCameraManagerClass=ALGPlayerCameraManager::StaticClass();
	SetShowMouseCursor(true);
}

FGenericTeamId ALGPlayerController::GetGenericTeamId() const
{
	/*int32 ID=1;
	if(TeamColor==ETeamColor::ETC_Blue)
	{
		ID=2;
	}
	else if(TeamColor==ETeamColor::ETC_Yellow)
	{
		ID=3;
	}
	return FGenericTeamId(ID);*/
	if(TeamColor==ETeamColor::ETC_Blue)
	{
		return TeamID_Blue;
	}
	else if(TeamColor==ETeamColor::ETC_Yellow)
	{
		return TeamID_Yellow;
	}
	return TeamID_Red;
}
