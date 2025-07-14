// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_CheckTargetDead.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Character/LGCharacterBase.h"

UBTService_CheckTargetDead::UBTService_CheckTargetDead()
{
	NodeName=TEXT("检查目标是否死亡");
	BlackboardKey.AddObjectFilter(this,TEXT("BlackboardKey"),ALGCharacterBase::StaticClass());
			// 这个父类里面已经有一个变量叫 BlackboardKey
}

void UBTService_CheckTargetDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(TargetKey.IsNone()||BlackboardKey.IsNone())
	{
		return;
	}
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID())))
	{
		if(CharacterBase->IsDead())
		{
			OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.GetSelectedKeyID());
		}
	}
	
}

void UBTService_CheckTargetDead::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBData=GetBlackboardAsset();
	if(BBData)
	{
		TargetKey.ResolveSelectedKey(*BBData);//将黑板键的名称转换为内部ID
		/*
		* // 解析后：
TargetKey 知道具体信息：
- 在黑板数组中的位置：比如索引[5]
- 数据类型：比如 UObject*
- 如何快速访问这个数据

		这行代码做了什么：
在黑板数据中查找你选择的键名
找到后记录它的索引位置
记录它的数据类型
建立快速访问路径
		 */
	}
}
