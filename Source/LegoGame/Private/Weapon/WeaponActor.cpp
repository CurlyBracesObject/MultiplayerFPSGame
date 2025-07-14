// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Character/LGCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "LegoGame/LegoGame.h"
#include "Net/UnrealNetwork.h"
#include "Player/LGPlayerCharacter.h"
#include "Sound/SoundCue.h"


// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SkeletalMeshComponent=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SetRootComponent(SkeletalMeshComponent);
	MaxClipVolume=30;
	FireInterval=0.5f;

	bReplicates=true;
	bNetUseOwnerRelevancy=true;
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	CurrentClipVolume=MaxClipVolume;
	
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponActor::SpawnBullet()
{
	if(CurrentClipVolume<=0)
	{
		return;
	}
	//生成效果
	//SpawnEffect();

	//生成伤害
	SpawnDamage();
	
	//记录当前次发射时间
	LastFireTime=GetWorld()->GetTimeSeconds();//保存了"此刻"的时间戳
	
	if((--CurrentClipVolume)>0)
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimeHandle,this,&AWeaponActor::SpawnBullet,FireInterval);
	}// 定时器句柄，用于后续引用或取消这个定时器
	else
	{
		CurrentState=EWeaponState::EWS_Empty;
	}
	OnRep_CurrentClipVolume();
}

void AWeaponActor::SpawnEffect()
{	if(FireSound)
	{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),FireSound,GetActorLocation());
	}
	if(FirePartical)
	{
		FVector MuzzlePos=SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleSocket"));
		FRotator MuzzleRot=SkeletalMeshComponent->GetSocketRotation(TEXT("MuzzleSocket"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),FirePartical,MuzzlePos,MuzzleRot);
	}
}

void AWeaponActor::SpawnDamage()
{
	//寻找发射射线的起点和发射射线的方向
	FVector Position;
	FVector Direction;
	GetFirePostAndDirection(Position,Direction);
	//发射检测射线
	FHitResult Hit;
	
	if(GetWorld()->LineTraceSingleByChannel(Hit,Position,Position+Direction*50000,ECC_WeaponTrace))
	{
		DrawDebugLine(GetWorld(),Position,Hit.Location,FColor::Green,false,3,0,2);
		//让撞击到目标受伤
		if(Hit.Actor.IsValid())
		{
			FPointDamageEvent Event;
			Event.HitInfo=Hit;
			Hit.Actor->TakeDamage(1,Event,MyMaster->GetController(),this);
		}
	}
	DrawDebugLine(GetWorld(),Position,Position + Direction*5000,FColor::Red,false,3);
}

void AWeaponActor::GetFirePostAndDirection(FVector& Position, FVector& Direction)
{
	Position=SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleSocket"));
	Direction=SkeletalMeshComponent->GetSocketRotation(TEXT("MuzzleSocket")).Vector();
	//如果当前操纵的是玩家
	if(Cast<ALGPlayerCharacter>(MyMaster))
	{
		if(APlayerController* PC=Cast<APlayerController>(MyMaster->GetController()))
		{
			//找屏幕中心点瞄准的位置
			int32 ScreenX,ScreenY;
			PC->GetViewportSize(ScreenX,ScreenY);//获取屏幕视口的尺寸，用于后续计算屏幕中心点。
			
			//转投坐标系 将屏幕坐标转换成三维空间坐标
			FVector WorldPos=MyMaster->GetCameraComponent()->GetComponentLocation();
			FVector WorldDir=MyMaster->GetCameraComponent()->GetComponentRotation().Vector();//用来存储从屏幕中心点转换得到的三维世界空间位置和方向。
																	//FRotator.Vector() 方法会将旋转转换为前向向量（Forward Vector）
			
			/*if(PC->DeprojectScreenPositionToWorld(ScreenX/2,ScreenY/2,WorldPos,WorldDir))
																	//WorldPos接收转换后的3D世界位置点  WorldDir接收从摄像机到该点的方向向量（归一化）
			{
				
			}*/
			//DrawDebugLine(GetWorld(),WorldPos,WorldPos+WorldDir*50000,FColor::Green,false,5);
			//发射检测射线
			FHitResult Hit;
			if(GetWorld()->LineTraceSingleByChannel(Hit,WorldPos,WorldPos+WorldDir*50000,ECC_WeaponTrace))
			{
				Direction=(Hit.Location-Position).GetSafeNormal();//向量标准化：将向量转换为长度为1的单位向量（保持方向不变）
				//计算从武器枪口到玩家实际瞄准点的精确方向,将这个方向向量标准化为单位向量（长度为1） 单位向量与原向量指向完全相同的方向
				//单位向量可以看作是原向量的"方向指示器"
				/*UE_LOG(LogTemp, Log, TEXT("====%s"),*Hit.Actor->GetName());
				DrawDebugLine(GetWorld(),Position,Hit.Location,FColor::Green,false,5);*/
			}
		}
	}
	else
	{
		//敌人的射击方向
		Direction=MyMaster->GetControlRotation().Vector();//这个计算的是哪里？敌人角色本身的Arrow的旋转方向？

		FHitResult Hit;

		//设置忽略目标
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(MyMaster);
		
		if(GetWorld()->LineTraceSingleByChannel(Hit,GetActorLocation(),GetActorLocation()+Direction*50000,ECC_WeaponTrace,Params))
		{
			Direction=(Hit.Location-Position).GetSafeNormal();//向量标准化：将向量转换为长度为1的单位向量（保持方向不变）
			//计算从武器枪口到玩家实际瞄准点的精确方向,将这个方向向量标准化为单位向量（长度为1） 单位向量与原向量指向完全相同的方向
			//单位向量可以看作是原向量的"方向指示器"
			/*UE_LOG(LogTemp, Log, TEXT("====%s"),*Hit.Actor->GetName());
			DrawDebugLine(GetWorld(),Position,Hit.Location,FColor::Green,false,5);*/
		}
	}
}

void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponActor,MyMaster);
	DOREPLIFETIME(AWeaponActor,CurrentClipVolume);
	DOREPLIFETIME(AWeaponActor,CurrentState);
}

void AWeaponActor::OnRep_CurrentClipVolume()
{
	if(CurrentClipVolume!=0 || CurrentClipVolume!=MaxClipVolume)//满足两种情况的其中之一
	{
		SpawnEffect();
	}
}

void AWeaponActor::Server_StartFire_Implementation()
{
	StartFire();
}

bool AWeaponActor::Server_StartFire_Validate()
{
	return true;
}

void AWeaponActor::NetMulti_PlayReloadAnim_Implementation()
{
	MyMaster->PlayAnimMontage(ReloadMontage);
}

void AWeaponActor::Server_StopFire_Implementation()
{
	StopFire();
}

bool AWeaponActor::Server_StopFire_Validate()
{
	return true;
}

void AWeaponActor::Server_Reload_Implementation()
{
	ReloadWeapon();
}

bool AWeaponActor::Server_Reload_Validate()
{
	return true;
}

void AWeaponActor::StartFire()
{
	if(!HasAuthority())
	{
		Server_StartFire();
		return;
	}
	if(CurrentState!=EWeaponState::EWS_Normal){return;}
	if(CurrentState==EWeaponState::EWS_Reload)
	{
		return;
	}
	CurrentState=EWeaponState::EWS_Fire;
	//UE_LOG(LogTemp, Log, TEXT("CurrentState=EWeaponState::EWS_Fire"))
	//检查当前冷却时间是否达到了
	if(GetWorld()->GetTimeSeconds()-LastFireTime>=FireInterval)
	{
		SpawnBullet();
	}
}


void AWeaponActor::StopFire()
{
	if(!HasAuthority())//不用GetOwner()->HasAuthority()?
	{
		Server_StopFire();
		return;
	}
	if(FireTimeHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimeHandle);
	}
	if(CurrentState==EWeaponState::EWS_Fire)
	{
		CurrentState=EWeaponState::EWS_Normal;
	}
}


void AWeaponActor::SetMaster(ALGCharacterBase* CharacterBase)
{
	//将枪械依附在角色手上
	SkeletalMeshComponent->AttachToComponent(CharacterBase->GetMesh(),FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("WeaponSocket"));
	MyMaster=CharacterBase;
	SetOwner(CharacterBase);
}

float AWeaponActor::ReloadWeapon()
{
	if(!HasAuthority())
	{
		Server_Reload();
		return 0;
	}
	if(CurrentState==EWeaponState::EWS_Reload)
	{
		return 0;
	}
	//if(CurrentClipVolume==MaxClipVolume){return 0;}
	if(CurrentState==EWeaponState::EWS_Fire)
	{
		StopFire();// 开火中，在开始换弹前停止射击
	}
	CurrentState=EWeaponState::EWS_Reload;
	if(ReloadMontage&&MyMaster)
	{
		//return MyMaster->PlayAnimMontage(ReloadMontage);
		//播放蒙太奇
		NetMulti_PlayReloadAnim();
		return ReloadMontage->GetPlayLength();
	}
	/*
	*UE_LOG(LogTemp, Warning, TEXT("无法播放ReloadMontage，ReloadMontage: %s, MyMaster: %s"), 
		   ReloadMontage ? TEXT("有效") : TEXT("无效"), 
		   MyMaster ? TEXT("有效") : TEXT("无效"));
	return 0;
	 */
	return 0;
}

void AWeaponActor::MakeFullClip()//换弹夹中
{
	CurrentClipVolume=MaxClipVolume;
	CurrentState=EWeaponState::EWS_Normal;
}

EWeaponState AWeaponActor::GetCurrentState()
{
	return CurrentState;
}



