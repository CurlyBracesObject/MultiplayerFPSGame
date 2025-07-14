// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
//#include "Weapon/WeaponActor.h"
#include "PropsSubsystem.generated.h"

class AWeaponActor;
UENUM()
enum class EPropsType:uint8
{
	EPT_SKin,
	EPT_Weapon
};

UENUM()
enum class ESkinType:uint8
{
	EST_None,
	EST_Hair,
	EST_Glasses,
	EST_Helmmet,
	EST_FaceMask,
	EST_Bag,
	EST_EyeMask,
	EST_Coat,
	EST_Beard
};


UENUM()
enum class EGender:uint8
{
	EG_Male,
	EG_Female
};


USTRUCT()
struct FPropsBase:public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	int32 ID;
	UPROPERTY(EditAnywhere)
	FText Name;
	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
	
	EPropsType Type;//不需要UPROPERTY(EditAnywhere)？不需要在编辑器中显示和编辑这个属性，则不需要UPROPERTY宏
							//想在蓝图或编辑器中访问它，应该添加UPROPERTY
};



#define WEAPON_INDEX 100

USTRUCT()
struct FSkin:public FPropsBase
{
	GENERATED_BODY()

	FSkin(): StaticMesh(nullptr), SkeletalMesh(nullptr), SkinType(ESkinType::EST_None), Gender(EGender::EG_Female)
	{
		Type = EPropsType::EPT_SKin;
	}

	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;
	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMesh;
	UPROPERTY(EditAnywhere)
	ESkinType SkinType;
	UPROPERTY(EditAnywhere)
	EGender Gender;
};

USTRUCT()
struct FWeapon:public FPropsBase
{
	FWeapon():WeaponMesh(nullptr)//为什么结构体里能写初始化？能写初始化的除了类 结构体 还有谁？
									//两者都可以有构造函数和初始化列表。区别主要在于结构体默认是公共的，而类默认是私有的
	{
		Type=EPropsType::EPT_Weapon;
	}
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	USkeletalMesh* WeaponMesh;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponActor> WeaponClass;
	
};

/**
 * 
 */
UCLASS()
class LEGOGAME_API UPropsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	const FPropsBase* GetPropsBaseByID(int32 ID);
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	UPROPERTY()
	UDataTable* SkinDataTable;
	UPROPERTY()
	UDataTable* WeaponDataTable;

	TMap<int32,FSkin*> SkinMap;
	TMap<int32,FWeapon*> WeaponMap;
};
