// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Character/LGCharacterBase.h"
#include "LGAIController.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGAIController : public AAIController
{
	GENERATED_BODY()
public:
	ALGAIController();
	
protected:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	UAIPerceptionComponent* GetPerceptionComponent(){return PerceptionComponent;}
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual FGenericTeamId GetGenericTeamId() const override;
	
	UPROPERTY(EditAnywhere)
	ETeamColor TeamColor;
	
};
