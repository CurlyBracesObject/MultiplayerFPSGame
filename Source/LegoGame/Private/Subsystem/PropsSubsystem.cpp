// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/PropsSubsystem.h"


const FPropsBase* UPropsSubsystem::GetPropsBaseByID(int32 ID)
{
	//UE_LOG(LogTemp, Log, TEXT("==%d"),ID);
	if(ID<WEAPON_INDEX)
	{
		if(SkinMap.Contains(ID))
		{
			return SkinMap[ID];
       	}
	}else
	{
		if(WeaponMap.Contains(ID))
	{
		return WeaponMap[ID];
	}
		
	}

	return nullptr;
}//ASceneItemActor里const FPropsBase* PropsBase=GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropBase(ID);
		//调用GetSubsystem 走FPropBase的Init SkinMap里不会为空
//FSkin 继承自 FPropsBase，SkinMap[ID] 返回的是 FPropsBase*，根据C++的向上转型规则，子类指针可以自动转换为父类指针

void UPropsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//加载数据表格
	SkinDataTable=LoadObject<UDataTable>(this,TEXT("DataTable'/Game/Lego/Data/Props/DT_Skin.DT_Skin'"));

	const TMap<FName,uint8*> Data=SkinDataTable->GetRowMap();
	//Key (FName)：行名称（不是数据列，是行的标识符） 这个Map的key自动找数据表格里的行命名NewRow、NewRow_0、NewRow_1、NewRow_2
			//*Value (uint8)：指向整行数据**的指针

	//遍历容器
	for(auto item:Data)
	{
		int ID=0;
		FString Left,Right;
		if(item.Key.ToString().Split(TEXT("_"),&Left,&Right))//String转整型
		{
			ID=FCString::Atoi(*Right)+1;
		}
		//检查是否已经存在
		if(!SkinMap.Contains(ID))
		{
			SkinMap.Add(ID, reinterpret_cast<FSkin*&&>(item.Value));//据表（DT_Skin）→ 加载到内存 → 转换为结构体 → 存储在Map中
			//reinterpret_cast将一种指针类型强制转换为另一种指针类型  将uint8*类型（通用字节指针）转换为FSkin*&&类型（FSkin指针的右值引用）
					//将数据表格中的原始数据转换为FSkin结构体 把编辑器里的数据表（DataTable）转换成代码中的结构体
						//reinterpret_cast<FSkin*>(Item.Value)将原始二进制数据 转换为 结构指针FSkin*
			//将通用的 uint8* 转换为具体的 FSkin* 类型
			//代码中可以直接访问结构体的属性
		}
	}

	//加载武器的表格数据
	WeaponDataTable=LoadObject<UDataTable>(this,TEXT("DataTable'/Game/Lego/Data/Props/DT_Weapon.DT_Weapon'"));

	const TMap<FName,uint8*> WeaponData=WeaponDataTable->GetRowMap();
	//遍历容器
	for(auto item:WeaponData)
	{
		int ID=WEAPON_INDEX;
		FString Left,Right;
		if(item.Key.ToString().Split(TEXT("_"),&Left,&Right))
		{
			ID=FCString::Atoi(*Right)+1+WEAPON_INDEX;
		}
		//检查是否已经存在
		if(!WeaponMap.Contains(ID))
		{
			WeaponMap.Add(ID, reinterpret_cast<FWeapon*&&>(item.Value));
			//reinterpret_cast将一种指针类型强制转换为另一种指针类型  将uint8*类型（通用字节指针）转换为FSkin*&&类型（FSkin指针的右值引用）
			//将数据表格中的原始数据转换为FSkin结构体
		}
	}
}
