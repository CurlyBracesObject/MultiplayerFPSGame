// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SkinComponent.h"

#include "Interface/SkinInterface.h"

// Sets default values for this component's properties
USkinComponent::USkinComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void USkinComponent::OnSkinPutOn(ESkinType Type, int32 ID)
{
	//UE_LOG(LogTemp, Log, TEXT("OnSkinPutOn"));
	if (!GetOwner() || !GetOwner()->GetRootComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("USkinComponent::OnSkinPutOn: Invalid owner or root component"));
		return;
	}
	const FPropsBase* PropsBase=GetOwner()->GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(ID);
	if(PropsBase&&PropsBase->Type==EPropsType::EPT_Weapon)
	{
		return;
	}
	const FSkin* Skin=static_cast<const FSkin*>(PropsBase);
	
	if(Skin->SkinType==ESkinType::EST_Coat)//如果是外套
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(Skin->SkeletalMesh);
	}else//静态网格
	{
		GetStaticMeshComponent(Type)->SetStaticMesh(Skin->StaticMesh);
		//是调用这个组件的SetStaticMesh方法，将Skin对象中的StaticMesh属性设置为这个组件要显示的网格模型。
	}
}


void USkinComponent::OnSkinTakeOff(ESkinType Type, int32 ID)// 清除相应组件上的网格模型的代码
	// 如果ID为-1，表示完全清除该类型的外观
{
	if(Type==ESkinType::EST_Coat)//如果是外套
	{
		GetSkeletalMeshComponent()->SetSkeletalMesh(nullptr);
	}else//静态网格
	{
		GetStaticMeshComponent(Type)->SetStaticMesh(nullptr);
	}
}


// Called when the game starts
void USkinComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

UStaticMeshComponent* USkinComponent::GetStaticMeshComponent(ESkinType Type)
{
	// 检查映射表中是否已存在此类型的组件
	if(StaticMeshCompMap.Contains(Type))
	{
		return StaticMeshCompMap[Type];
	}
	// 如果不存在，创建新组件 
		//第一次装备某类型的皮肤物品时，需要创建一个新的静态网格组件来显示这个物品
	UStaticMeshComponent* StaticMeshComponent=NewObject<UStaticMeshComponent>(GetOwner());
	StaticMeshComponent->RegisterComponentWithWorld(GetOwner()->GetWorld());//将这个组件注册到游戏世界中，使其能够正常工作
	//设置依附关系
	if(ISkinInterface* SkinInterface=Cast<ISkinInterface>(GetOwner()))
	{
		StaticMeshComponent->AttachToComponent(SkinInterface->GetSkinSkeletalMeshComponent(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			GetSocketName(Type));
	}
	// 将新创建的组件添加到映射表中 这个意思是放到StaticMeshCompMap里面吗？
	StaticMeshCompMap.Add(Type,StaticMeshComponent);

	return StaticMeshComponent;
}

USkeletalMeshComponent* USkinComponent::GetSkeletalMeshComponent()
{
	if(!SkeletalMeshComponent)
	{
		SkeletalMeshComponent=NewObject<USkeletalMeshComponent>(GetOwner());
		SkeletalMeshComponent->RegisterComponentWithWorld(GetOwner()->GetWorld());
		if(ISkinInterface* SkinInterface=Cast<ISkinInterface>(GetOwner()))
		{
			SkeletalMeshComponent->AttachToComponent(SkinInterface->GetSkinSkeletalMeshComponent(),FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			SkeletalMeshComponent->SetMasterPoseComponent(SkinInterface->GetSkinSkeletalMeshComponent());
		}
	}
	return SkeletalMeshComponent;
}

FName USkinComponent::GetSocketName(ESkinType Type)
{
	switch (Type)
	{
	case ESkinType::EST_Hair:
	case ESkinType::EST_Helmmet:
		return TEXT("HeadSocket");
	case ESkinType::EST_FaceMask:
	case ESkinType::EST_Beard:
	case ESkinType::EST_EyeMask:
	case ESkinType::EST_Glasses:
		return TEXT("GlassSocket");
	case ESkinType::EST_Bag:
		return TEXT("BagSocket");
	}
	return TEXT("None");
}


// Called every frame
void USkinComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

