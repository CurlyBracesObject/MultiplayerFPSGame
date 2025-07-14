// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Action/PawnAction_Reload.h"

#include "AIController.h"
#include "Character/LGCharacterBase.h"
#include "Weapon/WeaponActor.h"

	UPawnAction_Reload::UPawnAction_Reload()
{
	//bWantsTick=true;
	bBlockTask=true;
}

bool UPawnAction_Reload::Start()
{
	Super::Start();
	//获取角色
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(BrainComp->GetAIOwner()->GetPawn()))//运行Action的行为树组件
	{
		if(!CharacterBase->IsHoldWeapon())
		{
			return false;
		}
		float WaitTime=CharacterBase->GetHoldWeapon()->ReloadWeapon();
		//UE_LOG(LogTemp, Log, TEXT("ReloadWeapon返回WaitTime: %f"), WaitTime);
		if(!bBlockTask)
		{
			Finish(EPawnActionResult::Success);
		}
		else
		{
			//启用定时器
			if(WaitTime>0)
			{
				FTimerHandle Handle;
				BrainComp->GetAIOwner()->GetWorld()->GetTimerManager().SetTimer(Handle,this,&UPawnAction_Reload::ReloadEndCallBack,WaitTime);
			}
		}
	}
	
	/*UE_LOG(LogTemp, Log, TEXT("Start"))
	//Finish(EPawnActionResult::Success);
	//Finish(EPawnActionResult::InProgress);*/
	return true;
}

/*void UPawnAction_Reload::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("Tick"))
	//Super::Tick(DeltaTime);
}*/

/*void UPawnAction_Reload::Finish(TEnumAsByte<EPawnActionResult::Type> WithResult)
{
	UE_LOG(LogTemp, Log, TEXT("Finish"))
	Super::Finish(WithResult);
}*/

void UPawnAction_Reload::ReloadEndCallBack()
{
		if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(BrainComp->GetAIOwner()->GetPawn()))
		{
			if(CharacterBase->IsHoldWeapon())
			{
				CharacterBase->GetHoldWeapon()->MakeFullClip();
				UE_LOG(LogTemp, Log, TEXT("CurrentClipVolume:%05d"),CharacterBase->GetHoldWeapon()->GetCurrentClipVolume());
			}
		}
		
	Finish(EPawnActionResult::Success);
}
