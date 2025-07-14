// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystem/PropsSubsystem.h"
#include "PackageComponent.generated.h"

class ASceneItemActor;
class AWeaponActor;
//代理
DECLARE_MULTICAST_DELEGATE_OneParam(NotifySceneItem,ASceneItemActor*);//DelegateName ParamType
DECLARE_MULTICAST_DELEGATE_TwoParams(NotifyPackageItem,int32,int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(NotifySkinUpdate,ESkinType,int32);

DECLARE_MULTICAST_DELEGATE_OneParam(NotifyWeaponChangedDelegate,AWeaponActor*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEGOGAME_API UPackageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPackageComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int32 GetFreeSite();

	//Net
	UFUNCTION(NetMulticast,Reliable)
	void Multi_PutOnSkin(ESkinType Type,int32 ID);
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_PutOnSkin(ASceneItemActor* SceneItemActor,ESkinType Type);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_HoldWeapon();

	//Net
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_EquipWeapon(ASceneItemActor* SceneItemActor);
	UFUNCTION(NetMulticast,Reliable)
	void Multi_LockView(bool bLock);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddNearSceneItems(ASceneItemActor* SceneItemActor);
	void RemoveNearSceneItems(ASceneItemActor* SceneItemActor);

	//向背包中添加道具
	void AddItemToPackage(ASceneItemActor* SceneItemActor);
	
	void AddItemToPackage(int32 ID);
	void RemoveItemFromPackage(int32 Site,bool bSpawnSceneItem=true);
	
	TArray<ASceneItemActor*> GetNearSceneItems();

	//穿皮肤
	void PutOnSkin(ASceneItemActor* SceneItemActor,ESkinType SkinType=ESkinType::EST_None);
	
	void PutOnSkin(int32 Site,ESkinType SkinType=ESkinType::EST_None);//?不知道啥意思

	void TakeOffSkin(ESkinType Type,bool bUpdateSkin=true);

	void TakeOffSkinToScreen(ESkinType Type);

	//装备武器
	void EquipWeapon(ASceneItemActor* SceneItemActor);

	void EquipWeapon(int32 Site);

	bool EquipWeaponByPropsID(int32 ID);

	UFUNCTION(BlueprintCallable,DisplayName="EquipWeaponByPropsID")
	void K2_EquipWeaponByPropsID(int32 ID);

	void UnEquipWeapon(bool bToPackage=true);
	
	void SpawnNearSceneItem(int ID);

	AWeaponActor* GetHoldWeapon();


	int32 GetCurrentWeaponID(){return CurrentWeaponID;};
	
protected:
	UPROPERTY()
	TArray<ASceneItemActor*> NearSceneItems;

	TMap<int32,int32> Package;//装的道具在背包里的位置信息 应该是Key,那Value是啥？为啥需要两个ID？

	TMap<ESkinType,int32> SkinsMap;

	UPROPERTY(ReplicatedUsing=OnRep_HoldWeapon)
	AWeaponActor* HoldWeapon;//枪械：c++写逻辑 蓝图配置属性
	
public:
	//构建代理广播对象
	//附件有道具进入
	NotifySceneItem OnNearSceneItemEnter;
	//附近有道具离开
	NotifySceneItem OnNearSceneItemLeave;
	//背包道具被添加
	NotifyPackageItem OnPackageItemAdded;
	//背包道具被移除
	NotifyPackageItem OnPackageItemRemoved;
	//广播对象（委托）
		//不需要知道谁在监听，只负责发出通知
	//背包道具穿上了
	NotifySkinUpdate OnSkinPutOn;
	//通知皮肤脱下了
	NotifySkinUpdate OnSkinTakeOff;

	NotifyWeaponChangedDelegate OnEquipWeapon;
	NotifyWeaponChangedDelegate OnUnEquipWeapon;

	int32 CurrentWeaponID;
};
