// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LGHUD.generated.h"

class UPackageUserWidget;
/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGHUD : public AHUD
{
	GENERATED_BODY()

public:
	void TogglePackage();

	virtual void DrawHUD() override;
protected:
	UPROPERTY()
	UPackageUserWidget* PackageUI;
	
};
