// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Character/LGCharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "LGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGPlayerController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	ALGPlayerController();
protected:
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	ETeamColor TeamColor;
};
