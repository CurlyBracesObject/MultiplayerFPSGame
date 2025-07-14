// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackboardKey.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API UBTTask_ClearBlackboardKey : public UBTTask_BlackboardBase
//BTTask_BlackboardBase允许任务直接引用和操作黑板中的特定键值
{
	GENERATED_BODY()

public:
	UBTTask_ClearBlackboardKey();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
