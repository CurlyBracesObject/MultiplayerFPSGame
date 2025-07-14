// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LGCharacterBase.h"
#include "LGEnemyCharacter.generated.h"

class UBehaviorTree;
/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGEnemyCharacter : public ALGCharacterBase
{
	GENERATED_BODY()
public:
	ALGEnemyCharacter(const FObjectInitializer& ObjectInitializer);

	UBehaviorTree* GetBehaviorTree(){return BehaviorTree;};
protected:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
};
