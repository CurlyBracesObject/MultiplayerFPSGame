// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_LockTarget.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API UBTTask_LockTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_LockTarget();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere)
	bool bLockTarget;
};
