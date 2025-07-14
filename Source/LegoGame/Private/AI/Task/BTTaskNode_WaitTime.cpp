// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskNode_WaitTime.h"

UBTTaskNode_WaitTime::UBTTaskNode_WaitTime()
{
	NodeName=TEXT("等待时间");
	WaitTime=5.f;
	bNotifyTick=true;
	//bCreateNodeInstance=true;//告诉行为树系统  为该节点创建独立的实例数据  为每次使用此节点创建独立的内存空间
}

EBTNodeResult::Type UBTTaskNode_WaitTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)//系统分配给该节点实例的内存指针
{
	if(RandomDeviation>0)
	{//float* 是指向浮点数的指针     *()解引用 这个我能懂，因为我懂NodeMemory是地址 
		*((float*)NodeMemory)=FMath::FRandRange(WaitTime-RandomDeviation,WaitTime+RandomDeviation);
	}else
	{
		*((float*)NodeMemory)=WaitTime;
	}
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_WaitTime::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if((*((float*)NodeMemory)-=DeltaSeconds)<=0)
	{
		//继续执行行为树
		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
	}
}

uint16 UBTTaskNode_WaitTime::GetInstanceMemorySize() const
{
	return sizeof(float);//但是这种方式只能开辟一个变量的空间对吧？
}
