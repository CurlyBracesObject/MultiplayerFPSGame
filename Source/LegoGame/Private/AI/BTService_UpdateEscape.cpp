// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_UpdateEscape.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/LGCharacterBase.h"

UBTService_UpdateEscape::UBTService_UpdateEscape()
{
	NodeName=TEXT("更新逃亡状态");
	bCreateNodeInstance=true;
	EscapeTime=10.f;
	
}

void UBTService_UpdateEscape::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if((CalmTime-=DeltaSeconds)<=0)
	{
		//停止逃亡了
		OwnerComp.GetBlackboardComponent()->ClearValue(TEXT("Target"));
	}
	
}

void UBTService_UpdateEscape::OnInstanceCreated(UBehaviorTreeComponent& OwnerComp)
{
	Super::OnInstanceCreated(OwnerComp);
	CalmTime=EscapeTime;
	//绑定通知
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(CharacterBase->GetPackageComponent())
		{
			CharacterBase->OnDamaged.AddUObject(this,&UBTService_UpdateEscape::OnDamaged);
		}
		
	}
}
void UBTService_UpdateEscape::OnDamaged(ALGCharacterBase* CharacterBase)
{
	CalmTime=EscapeTime;

}
