// Fill out your copyright notice in the Description page of Project Settings.


#include "LGHUD.h"

#include "Character/LGCharacterBase.h"
#include "Engine/Canvas.h"
#include "UMG/PackageUserWidget.h"
#include "Weapon/WeaponActor.h"


void ALGHUD::TogglePackage()
{
	if(!PackageUI)
	{
		TSubclassOf<UPackageUserWidget> PackageClass=LoadClass<UPackageUserWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/UMG/UMG_Package.UMG_Package_C'"));
		PackageUI=CreateWidget<UPackageUserWidget>(GetOwningPlayerController(),PackageClass);
	}
	if(PackageUI && !PackageUI->IsInViewport())
	{
		PackageUI->AddToViewport();//// 实际上UI被添加到了 Viewport 这个根容器中
		// Viewport 就成为了 PackageUI 的 Parent
	}else
	{
		PackageUI->RemoveFromParent();
	}
	GetOwningPlayerController()->SetShowMouseCursor(PackageUI->IsInViewport());
}

void ALGHUD::DrawHUD()//每一帧都会调用这个函数
{
	Super::DrawHUD();
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwningPawn()))
	{
		if(CharacterBase->IsHoldWeapon())
		{
			//绘制准星
			float CenterX=Canvas->ClipX/2;
			float CenterY=Canvas->ClipY/2;

			float AimSignOffset=8;
			float AimSignLen=15;

			//上下
			DrawLine(CenterX,CenterY-AimSignOffset-AimSignLen,CenterX,CenterY-AimSignOffset,FLinearColor::Yellow);
			DrawLine(CenterX,CenterY+AimSignOffset+AimSignLen,CenterX,CenterY+AimSignOffset,FLinearColor::Yellow);

			//左右
			DrawLine(CenterX-AimSignOffset-AimSignLen,CenterY,CenterX-AimSignOffset,CenterY,FLinearColor::Yellow);
			DrawLine(CenterX+AimSignOffset+AimSignLen,CenterY,CenterX+AimSignOffset,CenterY,FLinearColor::Yellow);

			//绘制弹夹容积
			DrawText(FString::Printf(TEXT("%d/%d"),CharacterBase->GetHoldWeapon()->GetCurrentClipVolume(),CharacterBase->GetHoldWeapon()->GetMaxClipVolume()),FLinearColor::Red,
				200.f,200.f);

		}
	}
}
