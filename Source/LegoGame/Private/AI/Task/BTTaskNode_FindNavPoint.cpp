// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTaskNode_FindNavPoint.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTTaskNode_FindNavPoint::UBTTaskNode_FindNavPoint()
{
	NodeName=TEXT("寻找导航点");
	//设置黑板键值 拾取器 对于类型的约束
	TargetPosKey.AddVectorFilter(this,TEXT("TargetPosKey"));
	OriginPosKey.AddVectorFilter(this,TEXT("OriginPosKey"));
	OriginPosKey.AddObjectFilter(this,TEXT("OriginPosKey"),AActor::StaticClass());

	NavRadius=1000;
}

void UBTTaskNode_FindNavPoint::InitializeFromAsset(UBehaviorTree& Asset)//主要解析黑板键，将名称转换为ID
{
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBData=GetBlackboardAsset();
	if(BBData)
	{
		TargetPosKey.ResolveSelectedKey(*BBData);
		OriginPosKey.ResolveSelectedKey(*BBData);
	}
}

EBTNodeResult::Type UBTTaskNode_FindNavPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
//可能被多次调用（当行为树执行到根节点时，它会重新开始执行）    任务节点的实际功能逻辑
{
	//检查键值有效性
	if(OriginPosKey.IsNone()||TargetPosKey.IsNone())
	{
		return EBTNodeResult::Failed;
	}
	
	FVector OriginPos;
	FVector TargetPos;
	//检查键值拾取类型的方法
	if(OriginPosKey.SelectedKeyType==UBlackboardKeyType_Vector::StaticClass())
	{
		OriginPos=OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(OriginPosKey.GetSelectedKeyID());
	}
	else
	{
		if(AActor* Actor=Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(OriginPosKey.GetSelectedKeyID())))
		{
			OriginPos=Actor->GetActorLocation();
		}
	}
	//TargetPos=OriginPos+FRotator(0,FMath::RandRange(0,360),0).Vector()*FMath::RandRange(500,1000);
	//OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(TargetPosKey.GetSelectedKeyID(),TargetPos);
	//OwnerComp行为树组件拿黑板     将计算出的TargetPos值(一个位置坐标)
		//存储到黑板中名为TargetPosKey所指定的那个键里
	if(UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(OwnerComp.GetAIOwner(),OriginPos,TargetPos,NavRadius))
	{
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(TargetPosKey.GetSelectedKeyID(),TargetPos);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
	/*//基于调用者当前的位置 寻找一个随机位置
	if(AAIController* AICon=Cast<AAIController>(OwnerComp.GetOwner()))
	{
		FVector Pos=AICon->GetPawn()->GetActorLocation()+
		FRotator(0,FMath::RandRange(0,360),0).Vector()*FMath::RandRange(500,1000);

		//装填到黑板中
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("FindPos"),Pos);
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetPosKey,Pos);
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(TargetPosKey.GetSelectedKeyID(),Pos);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;*/
}

