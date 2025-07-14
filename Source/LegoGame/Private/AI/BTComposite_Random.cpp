// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTComposite_Random.h"

UBTComposite_Random::UBTComposite_Random()
{
	NodeName=TEXT("随机选取执行");
}

int32 UBTComposite_Random::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild,
	EBTNodeResult::Type LastResult) const
{
	/*int32 ChildIndex=FMath::RandRange(0,GetChildrenNum()-1);
	return ChildIndex;*/
	int32 NextChildIndex=BTSpecialChild::ReturnToParent;//不选择任何子节点，返回到父节点
	if(PrevChild==BTSpecialChild::NotInitialized)//第一次进入接着走   （非第一次进入时直接返回父节点）
	{
		NextChildIndex=FMath::RandRange(0,GetChildrenNum()-1);
	}
	return NextChildIndex;//告诉BT系统下一步要处理哪个节点
}
