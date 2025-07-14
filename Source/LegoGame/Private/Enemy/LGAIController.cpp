// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LGAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/LGEnemyCharacter.h"
#include "LegoGame/LegoGame.h"
#include "Perception/AIPerceptionComponent.h"

ALGAIController::ALGAIController()
{
	PerceptionComponent=CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
}

void ALGAIController::OnPossess(APawn* InPawn)//代表被 AI 控制器接管控制的游戏角色
{
	Super::OnPossess(InPawn);

	//检查当前控制的是EnemyCharacter吗 启用行为树
	if(ALGEnemyCharacter* EnemyCharacter=Cast<ALGEnemyCharacter>(InPawn))
	{
		RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
		//添加测试
		GetBlackboardComponent()->SetValueAsVector(TEXT("FindPos"),InPawn->GetActorLocation()+InPawn->GetActorForwardVector()*1000);
		GetBlackboardComponent()->SetValueAsVector(TEXT("OriginPos"),InPawn->GetActorLocation());
	}
}

ETeamAttitude::Type ALGAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	//方案1：任何实现了IGenericTeamAgentInterface的对象都可以参与团队系统
	//return Super::GetTeamAttitudeTowards(Other); 使用查找表
	// O(1) 时间复杂度
	if(const IGenericTeamAgentInterface* OtherTeamAgent=Cast<const IGenericTeamAgentInterface>(&Other))
	{
		if(IGenericTeamAgentInterface* MyTeamAgent=Cast<IGenericTeamAgentInterface>(GetPawn()))
		{
			return FGenericTeamId::GetAttitude(MyTeamAgent->GetGenericTeamId(),OtherTeamAgent->GetGenericTeamId());
		}
	}
	return ETeamAttitude::Neutral;
}
//方案2：只能处理ALGCharacterBase类型的对象 需要多次if-else判断
// 随着团队数量增加，性能会下降
/*ETeamAttitude::Type ALGAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if(const ALGCharacterBase* OtherCharacter=Cast<const ALGCharacterBase>(&Other))
		//进来的 到范围里面来的人 指的是AI 看起来逻辑是表现的这个意思 但是为什么不是指的是角色，指的是AI？
	{
		/*if(ALGCharacterBase* MyCharacter=Cast<ALGCharacterBase>(GetPawn()))//自己控制的角色
		{
			if(OtherCharacter->GetTeamColor()==ETeamColor::ETC_Yellow ||MyCharacter->GetTeamColor()==ETeamColor::ETC_Yellow)
			{
				return ETeamAttitude::Neutral;
			}
			return OtherCharacter->GetTeamColor()==MyCharacter->GetTeamColor()?ETeamAttitude::Friendly:ETeamAttitude::Hostile;
		}#1#
	}
	return ETeamAttitude::Neutral;
}*/

FGenericTeamId ALGAIController::GetGenericTeamId() const
{
	/*//return FGenericTeamId(11);
	int32 ID=1;
	if(TeamColor==ETeamColor::ETC_Blue)
	{
		ID=2;
	}
	else if(TeamColor==ETeamColor::ETC_Yellow)
	{
		ID=3;
	}
	return FGenericTeamId(ID);//函数返回类型是FGenericTeamId对象 //F前缀：通常是结构体(struct)，如FVector、FRotator*/
	if(TeamColor==ETeamColor::ETC_Blue)
	{
		return TeamID_Blue;
	}
	else if(TeamColor==ETeamColor::ETC_Yellow)
	{
		return TeamID_Yellow;
	}
	return TeamID_Red;
}
