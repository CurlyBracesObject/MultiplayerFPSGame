// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PackageComponent.h"

#include "Character/LGCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "Scene/SceneItemActor.h"
#include "Weapon/WeaponActor.h"

// Sets default values for this component's properties
UPackageComponent::UPackageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UPackageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

int32 UPackageComponent::GetFreeSite()
{
	int32 Site=0;
	while (Package.Contains(Site))
	{
		Site++;
	}
	return Site;
}

void UPackageComponent::Multi_PutOnSkin_Implementation(ESkinType Type, int32 ID)
{
	/*
	* 到这之前 人家有这个数据，
到了所有终端 再去检查下
万一没同步过去这个数据 会出问题
	 */
	//添加数据
	if(SkinsMap.Contains(Type))
	{
		SkinsMap[Type]=ID;
	}else
	{
		SkinsMap.Add(Type,ID);
	}

	//更新外观  //背包 广播下我脱了穿了一个装备
	if(OnSkinPutOn.IsBound())
	{
		OnSkinPutOn.Broadcast(Type,ID);
	}
	
}


void UPackageComponent::Server_PutOnSkin_Implementation(ASceneItemActor* SceneItemActor, ESkinType Type)
{
	PutOnSkin(SceneItemActor,Type);
}

bool UPackageComponent::Server_PutOnSkin_Validate(ASceneItemActor* SceneItemActor, ESkinType Type)
{
	return true;
}

void UPackageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPackageComponent,HoldWeapon);
}

void UPackageComponent::OnRep_HoldWeapon()
{
	
	if(HoldWeapon)
	{
		/*// 延迟0.1秒等待网络同步完成
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if(OnEquipWeapon.IsBound() && HoldWeapon)
			{
				OnEquipWeapon.Broadcast(HoldWeapon);
			}
		}, 1.f, false);*/

		if(OnEquipWeapon.IsBound())
    	{
    		OnEquipWeapon.Broadcast(HoldWeapon);
    	}
    
    	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwner()))
    	{
    		CharacterBase->SetLockView(true);
    	}	
	}
	else
	{
		
	}

}

void UPackageComponent::Server_EquipWeapon_Implementation(ASceneItemActor* SceneItemActor)
{
	EquipWeapon(SceneItemActor);
}

bool UPackageComponent::Server_EquipWeapon_Validate(ASceneItemActor* SceneItemActor)
{
	return true;
}


void UPackageComponent::Multi_LockView_Implementation(bool bLock)
{
	
}

// Called every frame
void UPackageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPackageComponent::AddNearSceneItems(ASceneItemActor* SceneItemActor)
{
	NearSceneItems.Add(SceneItemActor);
	if(OnNearSceneItemEnter.IsBound())//IsBound()判断当前代理是否已经被绑定了监听函数（是否有人“监听”它）
	{
		OnNearSceneItemEnter.Broadcast(SceneItemActor);//触发事件，所有监听者会收到通知
	}
}

void UPackageComponent::RemoveNearSceneItems(ASceneItemActor* SceneItemActor)
{
	NearSceneItems.Remove(SceneItemActor);
	if(OnNearSceneItemLeave.IsBound())
	{
		OnNearSceneItemLeave.Broadcast(SceneItemActor);
	}
}

void UPackageComponent::AddItemToPackage(ASceneItemActor* SceneItemActor)
{
	if(SceneItemActor)
	{
		SceneItemActor->Destroy();
		AddItemToPackage(SceneItemActor->GetID());
	}
}

void UPackageComponent::AddItemToPackage(int32 ID)
{
	int32 Site=GetFreeSite();
	Package.Add(Site,ID);
	if(OnPackageItemAdded.IsBound())
	{
		OnPackageItemAdded.Broadcast(Site,ID);
	}
}

void UPackageComponent::RemoveItemFromPackage(int32 Site,bool bSpawnSceneItem)
{
	if(Package.Contains(Site))//检测给定的位置是否包含道具
	{
		//广播
		if(OnPackageItemRemoved.IsBound())
		{
			OnPackageItemRemoved.Broadcast(Site,Package[Site]);
		}
		if(bSpawnSceneItem)
		{
			SpawnNearSceneItem(Package[Site]);
		}
		//SpawnNearSceneItem(Package[Site]);
		Package.Remove(Site);
	}
}

TArray<ASceneItemActor*> UPackageComponent::GetNearSceneItems()
{
	return NearSceneItems;
}

void UPackageComponent::PutOnSkin(ASceneItemActor* SceneItemActor,ESkinType SkinType)
{
	if(!SceneItemActor)
	{
		return;
	}
	if(!GetOwner()->HasAuthority())
	{
		Server_PutOnSkin(SceneItemActor,SkinType);
		return;
	}
	
	const FPropsBase* PropsBase=GetOwner()->GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(SceneItemActor->GetID());
	if(!PropsBase){return;}
	if(PropsBase->Type!=EPropsType::EPT_SKin){return;}
	//检测部件类别
	const FSkin* SkinInfo=static_cast<const FSkin*>(PropsBase);
	if(SkinType!=ESkinType::EST_None)
	{
		if(SkinType!=SkinInfo->SkinType)
		{
			return;
		}
	}
	SkinType=SkinInfo->SkinType;
	
	//如果之前有穿过部件皮肤，需要先脱掉
	TakeOffSkin(SkinType,false);


	Multi_PutOnSkin(SkinType,SceneItemActor->GetID());
	
	
	SceneItemActor->Destroy();
	
}

void UPackageComponent::PutOnSkin(int32 Site, ESkinType SkinType)
{
	if(!Package.Contains(Site)){return;}
	const FPropsBase* PropsBase=GetOwner()->GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(Package[Site]);
	if(!PropsBase){return;}
	if(PropsBase->Type!=EPropsType::EPT_SKin){return;}
	//检测部件类别
	const FSkin* SkinInfo=static_cast<const FSkin*>(PropsBase);
	if(SkinType!=ESkinType::EST_None)
	{
		if(SkinType!=SkinInfo->SkinType)
		{
			return;
		}
	}
	SkinType=SkinInfo->SkinType;
	
	//如果之前有穿过部件皮肤，需要先脱掉
	TakeOffSkin(SkinType,false);

	//添加数据
	SkinsMap.Add(SkinType,Package[Site]);

	//更新外观  //背包 广播下我脱了穿了一个装备
	if(OnSkinPutOn.IsBound())
	{
		OnSkinPutOn.Broadcast(SkinType,Package[Site]);
	}

	//移除背包的元素
	RemoveItemFromPackage(Site,false);
	
}


void UPackageComponent::TakeOffSkin(ESkinType Type, bool bUpdateSkin)
{
	if(SkinsMap.Contains(Type))//如果有则脱掉
	{
		AddItemToPackage(SkinsMap[Type]);//// 把旧物品添加回背包
		if(bUpdateSkin)
		{
			if(OnSkinTakeOff.IsBound())
			{
				OnSkinTakeOff.Broadcast(Type,-1);//这个到底是啥用的？
			}
		}
		SkinsMap.Remove(Type);
	}
}

void UPackageComponent::TakeOffSkinToScreen(ESkinType Type)
{
	if(SkinsMap.Contains(Type))//如果有则
		{
		SpawnNearSceneItem(SkinsMap[Type]);

		if(OnSkinTakeOff.IsBound())
		{
			OnSkinTakeOff.Broadcast(Type,-1);//这个到底是啥用的？通知监听者（如SkinComponent）某个类型的皮肤被脱下了 -1表示"无物品ID"，即告诉监听者该槽位现在为空
		}
		SkinsMap.Remove(Type);
		}
}

void UPackageComponent::EquipWeapon(ASceneItemActor* SceneItemActor)
{
	if(!GetOwner()->HasAuthority())
	{
		Server_EquipWeapon(SceneItemActor);
		return;
	}
	if(EquipWeaponByPropsID(SceneItemActor->GetID()))
	{
		SceneItemActor->Destroy();
	}
}

void UPackageComponent::EquipWeapon(int32 Site)
{
	if(Package.Contains(Site))
	{
		if(EquipWeaponByPropsID(Package[Site]))
		{
			RemoveItemFromPackage(Site,false);
		}
	}
}

bool UPackageComponent::EquipWeaponByPropsID(int32 ID)
{
	const FPropsBase* PropsBase=GetOwner()->GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(ID);
	if(!PropsBase || PropsBase->Type !=EPropsType::EPT_Weapon)
	{
		return false;
	}

	const FWeapon* WeaponInfo=static_cast<const FWeapon*>(PropsBase);

	UnEquipWeapon();

	//生成枪械
	HoldWeapon=GetOwner()->GetWorld()->SpawnActor<AWeaponActor>(WeaponInfo->WeaponClass);
	//设置主从关系
	if(HoldWeapon)//必须加这个吗？什么情况是HoldWeapon会是空的？
	{
		HoldWeapon->SetID(ID);
		//UE_LOG(LogTemp, Log, TEXT("SetID(%d), GetID()=%d"), ID, HoldWeapon->GetID());
		HoldWeapon->SetMaster(Cast<ALGCharacterBase>(GetOwner()));

		CurrentWeaponID=ID;//自己添加的新的变量 因为UI received WeaponID: 0，要改成WeaponID
		OnRep_HoldWeapon();
	}
	return true;
}

void UPackageComponent::K2_EquipWeaponByPropsID(int32 ID)
{
	EquipWeaponByPropsID(ID);
}

void UPackageComponent::UnEquipWeapon(bool bToPackage)
{
	if(HoldWeapon)
	{
		if(bToPackage)
		{
			AddItemToPackage(HoldWeapon->GetID());//不装备 手里正在拿的那把枪就放背包里
		}else
		{
			SpawnNearSceneItem(HoldWeapon->GetID());
		}
		
		HoldWeapon->Destroy();
		HoldWeapon=nullptr;//=nullptr 就是不指向任何地方的空指针了吗？
		if(OnUnEquipWeapon.IsBound())
		{
			OnUnEquipWeapon.Broadcast(nullptr);//为什么（）里面加了nullptr?
		}

		if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwner()))
		{
			CharacterBase->SetLockView(false);
		}
		
	}
}


void UPackageComponent::SpawnNearSceneItem(int ID)
{
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwner()))
	{
	FTransform Tran;
	Tran.SetLocation(CharacterBase->GetActorLocation()+
		FRotator(0,FMath::RandRange(0.f,360.f),0).Vector()+
		FMath::RandRange(50,150));
	ASceneItemActor* SceneItemActor=CharacterBase->GetWorld()->SpawnActorDeferred<ASceneItemActor>(ASceneItemActor::StaticClass(),Tran);//标记在内存中生成一个Actor，不会走beginPlay （构造 还是会走）
	SceneItemActor->SetID(ID);
	SceneItemActor->FinishSpawning(Tran);		
	}
	
}

AWeaponActor* UPackageComponent::GetHoldWeapon()
{
	return HoldWeapon;
}

