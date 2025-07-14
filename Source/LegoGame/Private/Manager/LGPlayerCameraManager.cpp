// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/LGPlayerCameraManager.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/LGPlayerCharacter.h"

void ALGPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	//调整相机吊臂的位置
	if(ALGPlayerCharacter* Player=Cast<ALGPlayerCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		Player->GetSpringArmComponent();
		if(Player->bIsCrouched && !bCrouched)
		{
			SpringArmZ=44;
		}else if(!Player->bIsCrouched && bCrouched)
		{
			SpringArmZ=-44;
		}
		bCrouched=Player->bIsCrouched;

		SpringArmZ=FMath::FInterpTo(SpringArmZ,0,DeltaTime,10);	
		Player->GetSpringArmComponent()->SetRelativeLocation(FVector(0,0,SpringArmZ));

		//调整持枪spring动画
		float CameraRelX=Player->GetCameraComponent()->GetRelativeLocation().X;
		float SpringSocketY=Player->GetSpringArmComponent()->SocketOffset.Y;
		float SpringTargetZ=Player->GetSpringArmComponent()->TargetOffset.Z;
		
		if(Player->IsHoldWeapon())
		{
			CameraRelX=FMath::FInterpTo(CameraRelX,150,DeltaTime,10);
			SpringTargetZ=FMath::FInterpTo(SpringTargetZ,50,DeltaTime,10);
			SpringSocketY=FMath::FInterpTo(SpringSocketY,80,DeltaTime,10);
		}else
		{
			CameraRelX=FMath::FInterpTo(CameraRelX,0,DeltaTime,10);
			SpringTargetZ=FMath::FInterpTo(SpringTargetZ,0,DeltaTime,10);
			SpringSocketY=FMath::FInterpTo(SpringSocketY,0,DeltaTime,10);
		}

		Player->GetCameraComponent()->SetRelativeLocation(FVector(CameraRelX,0,0));
		Player->GetSpringArmComponent()->SocketOffset.Y=SpringSocketY;
		Player->GetSpringArmComponent()->TargetOffset.Z=SpringTargetZ;

		float TargetFOV=90;
		if(Player->IsHoldWeapon()&&Player->IsIronSight())
		{
			TargetFOV=60;
		}
		DefaultFOV=FMath::FInterpTo(DefaultFOV,TargetFOV,DeltaTime,10);
		SetFOV(DefaultFOV);
	}
	Super::UpdateCamera(DeltaTime);//有的时候不知道 应不应该在前面还是后面写代码？
	
}
