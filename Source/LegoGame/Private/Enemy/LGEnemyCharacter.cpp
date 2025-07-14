// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LGEnemyCharacter.h"


#include "Enemy/LGAIController.h"
#include "BehaviorTree/BehaviorTree.h"

ALGEnemyCharacter::ALGEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AIControllerClass=ALGAIController::StaticClass();
	AutoPossessAI=EAutoPossessAI::PlacedInWorldOrSpawned;
}
