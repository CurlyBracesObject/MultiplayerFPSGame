// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_Random.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API UBTComposite_Random : public UBTCompositeNode
{
	GENERATED_BODY()

public:
	UBTComposite_Random();

protected:
	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
};
