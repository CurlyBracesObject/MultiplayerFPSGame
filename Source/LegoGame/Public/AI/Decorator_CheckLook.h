// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "Decorator_CheckLook.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API UDecorator_CheckLook : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UDecorator_CheckLook();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere)
	float TargetOffset;
	UPROPERTY(EditAnywhere)
	float OriginOffset;
	UPROPERTY(EditAnywhere)
	float DrawDebugLineTime;
};
