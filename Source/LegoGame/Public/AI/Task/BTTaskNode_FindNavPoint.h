// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindNavPoint.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API UBTTaskNode_FindNavPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_FindNavPoint();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
//将键值拾取器和黑板进行关联  和使用的人关联
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector OriginPosKey;
	UPROPERTY(EditAnywhere)
	//FName TargetPosKey;
	FBlackboardKeySelector TargetPosKey;
	UPROPERTY(EditAnywhere)
	float NavRadius;
};
