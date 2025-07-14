// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator_CheckLook.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "LegoGame/LegoGame.h"

UDecorator_CheckLook::UDecorator_CheckLook()
{
	NodeName=TEXT("检查是可以看到目标玩家");
	BlackboardKey.AddObjectFilter(this,TEXT("BlackboardKey"),AActor::StaticClass());
}

bool UDecorator_CheckLook::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(BlackboardKey.IsNone())
	{
		return false;
	}
	FVector StartPos=OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	FVector EndPos;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerComp.GetAIOwner()->GetPawn());
	
	if(AActor* Actor=Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID())))
	{
		EndPos=Actor->GetActorLocation();
		Params.AddIgnoredActor(Actor);
	}
	else
	{
		return false;
	}

	FHitResult Hit;
	if(DrawDebugLineTime>0)
	{
		DrawDebugLine(OwnerComp.GetAIOwner()->GetWorld(),StartPos+FVector::UpVector*OriginOffset,EndPos+FVector::UpVector*TargetOffset,FColor::Yellow,false,DrawDebugLineTime);
	}//代表AI向目标发射的视线检测射线 不管有没有障碍物都会显示这条线
	
	return !OwnerComp.GetAIOwner()->GetWorld()->LineTraceSingleByChannel(Hit,StartPos+FVector::UpVector*OriginOffset,EndPos+FVector::UpVector*TargetOffset,ECC_WeaponTrace,Params);
	//返回true表示击中了障碍物 Params.AddIgnoredActor这里忽视了玩家
		//我们要判断是否击中玩家 所以要取反 表示击中了玩家
		



	/*没有！
	//射线没碰到任何东西（直达目标） ，return false = 射线没击中障碍物
	 *击中=true，碰到障碍物，表示没有触到玩家
	//false是一路没有障碍物，看得到玩家  这样就跳过找点逻辑 直接执行后面的开火逻辑
		//	目的是 判断是否能看到玩家，能看到玩家就在原地射击 不要找点（Fire的时候找一个点）*/
}
