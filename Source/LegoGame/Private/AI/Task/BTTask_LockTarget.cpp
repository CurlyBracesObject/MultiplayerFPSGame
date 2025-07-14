// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_LockTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTTask_LockTarget::UBTTask_LockTarget()
{
	NodeName=TEXT("锁定目标");
	BlackboardKey.AddObjectFilter(this,TEXT("BlackboardKey"),AActor::StaticClass());
	bLockTarget=true;
}

EBTNodeResult::Type UBTTask_LockTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(bLockTarget)
	{
		if(BlackboardKey.IsNone())
		{
			return EBTNodeResult::Failed;
		}
		if(AActor* TargetActor=Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID())))
		{
			OwnerComp.GetAIOwner()->SetFocus(TargetActor);
		}
	}
	else
	{
		OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
	}
	return EBTNodeResult::Succeeded;
}
