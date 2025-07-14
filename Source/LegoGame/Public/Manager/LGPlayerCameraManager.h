// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LGPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
protected:
	void UpdateCamera(float DeltaTime) override;

private:
	float SpringArmZ;
	uint8 bCrouched;
};
