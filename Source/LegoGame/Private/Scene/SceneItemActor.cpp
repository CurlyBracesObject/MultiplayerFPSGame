// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/SceneItemActor.h"

#include "Components/BillboardComponent.h"
#include "LegoGame/LegoGame.h"
#include "Subsystem/PropsSubsystem.h"

// Sets default values
ASceneItemActor::ASceneItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick =false;
	RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	BillboardComponent=CreateDefaultSubobject<UBillboardComponent>(TEXT("BillbordComp"));
	BillboardComponent->SetupAttachment(RootComponent);
	bReplicates=true;
}

// Called when the game starts or when spawned
void ASceneItemActor::BeginPlay()
{
	Super::BeginPlay();
	InitActor();
	/*// 确保碰撞设置正确
	if(StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
		StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		StaticMeshComponent->SetGenerateOverlapEvents(true);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		UE_LOG(LogTemp, Warning, TEXT("SceneItemActor %s collision setup complete"), *GetName());
	}*/
	// 确保所有组件都使用正确的碰撞设置
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for(UPrimitiveComponent* Comp : Components)
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Comp->SetGenerateOverlapEvents(true);

		Comp->SetCollisionObjectType(ECC_SceneItem);
		
		//Comp->SetCollisionResponseToAllChannels(ECR_Ignore);
	    Comp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Comp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
		Comp->SetCollisionResponseToChannel(ECC_WeaponTrace, ECR_Ignore);

	}
	
}

void ASceneItemActor::InitActor()
{
	//找一下数据对象
	const FPropsBase* PropsBase=GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(ID);
	if(PropsBase)
	{
		if(PropsBase->Type==EPropsType::EPT_SKin)
		{
			const FSkin* Skin= static_cast<const FSkin*>(PropsBase);
					//将父类指针转换回子类指针
			if(Skin->StaticMesh)
			{
				GetStaticMeshComponent()->SetStaticMesh(Skin->StaticMesh);
				//UE的UStaticMeshComponent内置里有个方法SetStaticMesh（UStaticMesh* StaticMesh）
					//GetStaticMeshComponent()返回UStaticMeshComponent*
					//Skin->StaticMesh返回UStaticMesh*
			}else if(Skin->SkeletalMesh)
			{
				GetSkeletalMeshComponent()->SetSkeletalMesh(Skin->SkeletalMesh);
			}
		}else if(PropsBase->Type==EPropsType::EPT_Weapon)
		{
			const FWeapon* Weapon=static_cast<const FWeapon*>(PropsBase);
			GetSkeletalMeshComponent()->SetSkeletalMesh(Weapon->WeaponMesh);
		}
	}
}

UStaticMeshComponent* ASceneItemActor::GetStaticMeshComponent()
{
	if(StaticMeshComponent)
	{
		return StaticMeshComponent;
	}
	//动态创建组件
	StaticMeshComponent=NewObject<UStaticMeshComponent>(this);//动态创建组件  运行后此Actor才生成StaticMeshComponent
	StaticMeshComponent->RegisterComponentWithWorld(GetWorld());
	StaticMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	/*// 设置碰撞属性
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
    
	UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent created with overlap settings"));*/
	//设置collision的物体类别通道
	//StaticMeshComponent->SetCollisionObjectType(ECC_SceneItem);
			//这样做的目的是将这些对象归类为一类特殊的物体，便于统一管理它们的碰撞行为。
	/*StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	StaticMeshComponent->SetGenerateOverlapEvents(true);*/
	//StaticMeshComponent->SetCollisionObjectType(ECC_SceneItem);
	return StaticMeshComponent;
}

USkeletalMeshComponent* ASceneItemActor::GetSkeletalMeshComponent()
{
	if(!SkeletalMeshComponent)
	{
		SkeletalMeshComponent=NewObject<USkeletalMeshComponent>(this);//动态创建组件  运行后此Actor才生成StaticMeshComponent
		SkeletalMeshComponent->RegisterComponentWithWorld(GetWorld());
		SkeletalMeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//之后加的 可以去掉
		//SkeletalMeshComponent->SetCollisionObjectType(ECC_SceneItem);
		
		//SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
		//SkeletalMeshComponent->SetGenerateOverlapEvents(true);

	}
	return SkeletalMeshComponent;
}


// Called every frame
void ASceneItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

