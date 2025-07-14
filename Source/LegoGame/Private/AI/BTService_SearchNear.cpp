// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_SearchNear.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/LGCharacterBase.h"
#include "Components/PackageComponent.h"
#include "LegoGame/LegoGame.h"
#include "Scene/SceneItemActor.h"

UBTService_SearchNear::UBTService_SearchNear()
{
	NodeName=TEXT("搜寻附近");
	SearchRadius=150.f;
	bCreateNodeInstance=true;
}

void UBTService_SearchNear::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	//检索附近有无道具，如果有则拾取道具
	//用来存储检测目标
	TArray<FHitResult> Hits;
	//检测形状
	FCollisionShape Shape;
	Shape.ShapeType=ECollisionShape::Sphere;
	Shape.Sphere.Radius=SearchRadius;
	//检测起点
	FVector Position=OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	//UE_LOG(LogTemp, Log, TEXT("======a %s"),*Position.ToString());
	//添加询问参数
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerComp.GetAIOwner()->GetPawn());
	
	DrawDebugSphere(OwnerComp.GetAIOwner()->GetWorld(),Position,SearchRadius,8,FColor::Red,false,3.f);

	/*if(GetWorld()->SweepMultiByChannel(Hits,Position,Position,
		FRotator::ZeroRotator.Quaternion(),
		ECC_Visibility,Shape,Params))*/
	if(GetWorld()->SweepMultiByObjectType(Hits,Position,Position,
		FRotator::ZeroRotator.Quaternion(),
		ECC_SceneItem,Shape,Params))
	{
		for(auto Hit:Hits)
		{
			if(ASceneItemActor* ItemActor=Cast<ASceneItemActor>(Hit.Actor))
			{
				//拾取道具
				if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn()))
				{
					if(CharacterBase->GetPackageComponent())
					{
						if(ItemActor->GetID()<WEAPON_INDEX)
						{
							CharacterBase->GetPackageComponent()->PutOnSkin(ItemActor);
						}
						else
						{
							CharacterBase->GetPackageComponent()->EquipWeapon(ItemActor);
						}
					}
				}
				break;

			}

		}
		
	}
}


void UBTService_SearchNear::OnInstanceCreated(UBehaviorTreeComponent& OwnerComp)//每次进入包含此服务的节点时都会创建一个新实例并调用此函数
{
	Super::OnInstanceCreated(OwnerComp);
	//绑定通知
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(CharacterBase->GetPackageComponent())
		{
			CharacterBase->GetPackageComponent()->OnEquipWeapon.AddUObject(this,&UBTService_SearchNear::OnEquipWeapon);
			CharacterBase->OnDamaged.AddUObject(this,&UBTService_SearchNear::OnDamaged);
		}
		MyCharacterBase=CharacterBase;
	}
}

void UBTService_SearchNear::OnEquipWeapon(AWeaponActor* WeaponActor)
{
	//UE_LOG(LogTemp, Log, TEXT("拾取到武器了"))
	if(MyCharacterBase)
	{
		if(AAIController* AICon=Cast<AAIController>(MyCharacterBase->GetController()))
		{
			AICon->GetBlackboardComponent()->SetValueAsBool(TEXT("bHoldWeapon"),true);
		}
	}
}

void UBTService_SearchNear::OnDamaged(ALGCharacterBase* CharacterBase)
{
	if(CharacterBase)
	{
		if(AAIController* AICon=Cast<AAIController>(MyCharacterBase->GetController()))
		{
			AICon->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"),CharacterBase);
		}
	}
	
}
