// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/LGAnimInstance.h"
#include "Character/LGCharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Weapon/WeaponActor.h"


ULGAnimInstance::ULGAnimInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	WaitRelaxed=4.f;
}

void ULGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!MyCharacter)
	{
		//获取当前使用此动画实例的角色
		MyCharacter=Cast<ALGCharacterBase>(TryGetPawnOwner());
		return;
	}
	Speed=MyCharacter->GetVelocity().Size2D();// 计算速度在水平面上的大小  忽略Z分量（垂直方向）

	//UE_LOG(LogTemp, Log, TEXT("Speed %f"),Speed);
	
	bSprinting=MyCharacter->IsSprinting()&&Speed>0;//为啥还需要Speed>0双重保险判断吗？按了shift后改变了IsSprinting()的值（这个时候速度不一定是大于0，只按shift 还得按个WASD）//如果玩家按住冲刺键但站着不动，角色不应显示冲刺动画
	bCrouch=MyCharacter->bIsCrouched;

	bHoldWeapon=MyCharacter->IsHoldWeapon();

	bJump=MyCharacter->GetMovementComponent()->IsFalling();
	
	if(Speed>0)
	{
		/*//计算移动角度
		float CosValue=FVector::DotProduct(MyCharacter->GetVelocity().GetSafeNormal(),MyCharacter->GetActorForwardVector());
		Direction=FMath::Acos(CosValue)*180/3.14f;
		//检查是否朝左移动 如果朝左移动则将方向乘以-1
		if(FVector::DotProduct(MyCharacter->GetVelocity().GetSafeNormal(),MyCharacter->GetActorRightVector())<0)
		{
			Direction*=-1;
		}*/
		Direction=CalculateDirection(MyCharacter->GetVelocity(),MyCharacter->GetControlRotation());
	}

	if(bHoldWeapon)
	{
		//AimPitch=MyCharacter->GetControlRotation().GetNormalized().Pitch;
		AimPitch=MyCharacter->GetBaseAimRotation().GetNormalized().Pitch;
		bIronSight=MyCharacter->IsIronSight();

		HoldWeaponPos=MyCharacter->GetHoldWeapon()->GetSkeletalMeshComponent()->GetSocketLocation(TEXT("HoldSocket"));

		bFire=MyCharacter->GetHoldWeapon()->GetCurrentState()==EWeaponState::EWS_Fire;
	}else
	{
		//更新放松状态
		if((WaitRelaxed-=DeltaSeconds)<0)
		{
			bRelaxed=true;
		}
		if(Speed>0||bCrouch||bJump)
		{
			bRelaxed=false;
			WaitRelaxed=4.f;
		}
	}

}

void ULGAnimInstance::AnimNotify_RelaxedEnd()
{
	bRelaxed=false;
	WaitRelaxed=4.f;
}

/*
void ULGAnimInstance::AnimNotify_WeaponReload()
{
	UE_LOG(LogTemp, Log, TEXT("qqqqqqqqqqqqqqqqq"))
}
*/
