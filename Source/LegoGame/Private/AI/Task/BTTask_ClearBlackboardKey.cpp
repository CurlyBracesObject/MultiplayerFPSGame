// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_ClearBlackboardKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardKey::UBTTask_ClearBlackboardKey()
{
	NodeName=TEXT("清理黑板键值");
}

EBTNodeResult::Type UBTTask_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(BlackboardKey.IsNone())
	{
		return EBTNodeResult::Failed;
	}
	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.GetSelectedKeyID());
	return EBTNodeResult::Succeeded;
}
