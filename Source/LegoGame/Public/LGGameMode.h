// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LGGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGGameMode : public AGameMode
{
	GENERATED_BODY()

	ALGGameMode();

protected:
	virtual void BeginPlay() override;
};
