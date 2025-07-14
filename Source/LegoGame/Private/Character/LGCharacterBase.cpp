// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LGCharacterBase.h"

#include "Components/BillboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LGCharacterMovementComponent.h"
#include "Components/PackageComponent.h"
#include "Components/SkinComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "LegoGame/LegoGame.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/WeaponActor.h"

// Sets default values
ALGCharacterBase::ALGCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<ULGCharacterMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PackageComponent=CreateDefaultSubobject<UPackageComponent>(TEXT("PackageComp"));

	SkinComponent=CreateDefaultSubobject<USkinComponent>(TEXT("SkinComp"));

	BillboardComponent=CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComp"));
	BillboardComponent->SetupAttachment(RootComponent);
	BillboardComponent->SetRelativeLocation(FVector(0,0,160));

	MaxHP=300;
}


// Called when the game starts or when spawned
void ALGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: %s, Controller: %s"), 
			  *GetName(), 
			  GetController() ? *GetController()->GetName() : TEXT("NULL"));

	
	//绑定通知
	PackageComponent->OnSkinPutOn.AddUObject(SkinComponent,&USkinComponent::OnSkinPutOn);
	PackageComponent->OnSkinTakeOff.AddUObject(SkinComponent,&USkinComponent::OnSkinTakeOff);

	CurrentHP=MaxHP;
}

void ALGCharacterBase::StartSprint()
{
	bSprint=true;
	//if(IsHoldWeapon()){StopFire();}
	if(!HasAuthority())
	{
		Server_ChangeSprint(true);
	}
}



void ALGCharacterBase::StopSprint()
{
	bSprint=false;
	if(!HasAuthority())
	{
		Server_ChangeSprint(false);
	}
}

void ALGCharacterBase::DoCrouch()
{
	// 确保允许下蹲
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if(MovementComp && !MovementComp->CanEverCrouch())
	{
		// 获取可修改的引用
		FNavAgentProperties& NavProps = MovementComp->GetNavAgentPropertiesRef();
		NavProps.bCanCrouch = true;
	}
	
	if( CanCrouch())
	{
		//UE_LOG(LogTemp, Log, TEXT("进行crouch"))
		Crouch();
	}
	else
	{
		UnCrouch();
	}
	/*// 详细诊断
	UE_LOG(LogTemp, Warning, TEXT("======= 下蹲诊断开始 ======="));
	UE_LOG(LogTemp, Warning, TEXT("bIsCrouched当前值: %s"), bIsCrouched ? TEXT("true") : TEXT("false"));
   
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if(MovementComp)
	{
		// 检查NavAgentProps
		UE_LOG(LogTemp, Warning, TEXT("NavAgentProps.bCanCrouch: %s"), 
		   MovementComp->GetNavAgentPropertiesRef().bCanCrouch ? TEXT("true") : TEXT("false"));
      
		// 如果NavAgentProps.bCanCrouch为false，直接修改它
		if(!MovementComp->GetNavAgentPropertiesRef().bCanCrouch)
		{
			// 获取可修改的引用
			FNavAgentProperties& NavProps = MovementComp->GetNavAgentPropertiesRef();
			NavProps.bCanCrouch = true;
			UE_LOG(LogTemp, Warning, TEXT("已设置NavAgentProps.bCanCrouch = true"));
		}
      
		UE_LOG(LogTemp, Warning, TEXT("修改后CanEverCrouch(): %s"), 
		   MovementComp->CanEverCrouch() ? TEXT("true") : TEXT("false"));
		UE_LOG(LogTemp, Warning, TEXT("当前移动模式: %d"), (int)MovementComp->MovementMode);
	}
   
	UE_LOG(LogTemp, Warning, TEXT("是否持有武器: %s"), IsHoldWeapon() ? TEXT("是") : TEXT("否"));
	UE_LOG(LogTemp, Warning, TEXT("======= 下蹲诊断结束 ======="));

	// 原有代码
	bool canCrouchNow = CanCrouch();
	UE_LOG(LogTemp, Warning, TEXT("CanCrouch返回值: %s"), canCrouchNow ? TEXT("true") : TEXT("false"));
   
	if(canCrouchNow)
	{
		UE_LOG(LogTemp, Warning, TEXT("进行crouch"));
		Crouch();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("不能crouch，尝试UnCrouch"));
		UnCrouch();
	}*/
}


USkeletalMeshComponent* ALGCharacterBase::GetSkinSkeletalMeshComponent()
{
	return GetMesh();
}

void ALGCharacterBase::StartIronSight()
{
	if(!HasAuthority())
	{
		Server_StartIronSight();
		return;
	}
	bIronSight=true;
}

void ALGCharacterBase::StopIronSight()
{
	if(!HasAuthority())
	{
		Server_StopIronSight();
		return;
	}
	bIronSight=false;
}

float ALGCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(CurrentHP<=0)
	{
		return 0;
	}
	CurrentHP--;
	if(CurrentHP<=0)
	{
		//生成观测对象
		if(Cast<APlayerController>(GetController()))
		{
			TSubclassOf<ASpectatorPawn> SpectatorcClass=LoadClass<ASpectatorPawn>(nullptr,TEXT("Blueprint'/Game/Lego/BP/Player/BP_Spectator.BP_Spectator_C'"));
            		ASpectatorPawn* SpectatorPawn=GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorcClass,GetActorLocation(),GetActorRotation());
            		if(SpectatorPawn)
            		{
            			GetController()->Possess(SpectatorPawn);
            		}else
            		{
            			//解除控制
            			GetController()->UnPossess();//角色不再响应AI逻辑变成纯物理对象
            		}
		}else
		{
			//解除控制
			GetController()->UnPossess();//角色不再响应AI逻辑变成纯物理对象
		}
		
				
		//添加冲力
		const FPointDamageEvent* PointDamageEvent=static_cast<const FPointDamageEvent*>(&DamageEvent);
		//枪械到击中点
		FVector ForceDir=(PointDamageEvent->HitInfo.Location-DamageCauser->GetActorLocation()).GetSafeNormal();

		Multi_Dead(ForceDir,PointDamageEvent->HitInfo.BoneName);
	}
	//UE_LOG(LogTemp, Log, TEXT("我（%s）被（%s）攻击了！"),*GetName(),*EventInstigator->GetPawn()->GetName());

	if(OnDamaged.IsBound())
	{
		OnDamaged.Broadcast(Cast<ALGCharacterBase>(EventInstigator->GetPawn()));
	}
	
	return 0;
}

void ALGCharacterBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UE_LOG(LogTemp, Warning, TEXT("OnConstruction: %s, Controller: %s"), 
		   *GetName(), 
		   GetController() ? *GetController()->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("1"));
	//OnConstruction在构造时调用，此时GetController()可能返回NULL
		//依赖Controller意味着必须先设置好Controller才能获取团队信息
			//直接在Character中存储团队信息更直接，减少依赖链
	/*if(IGenericTeamAgentInterface* TeamInterface=Cast<IGenericTeamAgentInterface>(GetController()))
		//GetController()获取角色的控制器，然后转换为IGenericTeamAgentInterface接口
	{
		UE_LOG(LogTemp, Log, TEXT("2"));
		/#1#/根据自己当前的阵营标记，更改广告牌的内容
		if(BillboardComponent)
		{
			TCHAR* Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_Crossbow.icon_weapon_Crossbow'");
			FGenericTeamId TeamId=TeamInterface->GetGenericTeamId();
									//接口多态 接口指针调用的是控制器实现的接口方法
			if(TeamId.GetId()==1)
			{
				Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_Apple.icon_weapon_Apple'");
			}
			else if(TeamId.GetId()==2)
			{
				Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_FlashBang.icon_weapon_FlashBang'");
			}
			UTexture2D* Icon=LoadObject<UTexture2D>(this,Path);
			BillboardComponent->SetSprite(Icon);#1#
		}*/
	if(BillboardComponent)
	{
		TCHAR* Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_Crossbow.icon_weapon_Crossbow'");
		FGenericTeamId TeamId=GetGenericTeamId();
		//接口多态 接口指针调用的是控制器实现的接口方法
		if(TeamId.GetId()==1)
		{
			Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_Apple.icon_weapon_Apple'");
		}
		else if(TeamId.GetId()==2)
		{
			Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_FlashBang.icon_weapon_FlashBang'");
		}
		UTexture2D* Icon=LoadObject<UTexture2D>(this,Path);
		BillboardComponent->SetSprite(Icon);
	}
	/*//根据自己当前的阵营标记，更改广告牌的内容
	if(BillboardComponent)
	{
		TCHAR* Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_Crossbow.icon_weapon_Crossbow'");
		if(TeamColor==ETeamColor::ETC_Red)
		{
			Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_Apple.icon_weapon_Apple'");
		}
		else if(TeamColor==ETeamColor::ETC_Blue)
		{
			Path=TEXT("Texture2D'/Game/Lego/Textures/ICons/icon_weapon_FlashBang.icon_weapon_FlashBang'");
		}
		UTexture2D* Icon=LoadObject<UTexture2D>(this,Path);
		BillboardComponent->SetSprite(Icon);
	}*/
}

FGenericTeamId ALGCharacterBase::GetGenericTeamId() const
{
	/*//return FGenericTeamId(10);
	if(IGenericTeamAgentInterface* TeamInterface=Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return TeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;//? 这个结构体为什么有no team*///为什么不能用上面的逻辑了
	if(TeamColor==ETeamColor::ETC_Blue)
	{
		return TeamID_Blue;
	}
	else if(TeamColor==ETeamColor::ETC_Yellow)
	{
		return TeamID_Yellow;
	}
	return TeamID_Red;
}

void ALGCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer=LGTagContainer;
}

bool ALGCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return LGTagContainer.HasAll(TagContainer);
}

bool ALGCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return LGTagContainer.HasAny(TagContainer);
}

void ALGCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALGCharacterBase,bSprint);
	DOREPLIFETIME(ALGCharacterBase,bIronSight);
}

void ALGCharacterBase::Server_StartIronSight_Implementation()
{
	StartIronSight();
}

bool ALGCharacterBase::Server_StartIronSight_Validate()
{
	return true;
}

void ALGCharacterBase::Server_ChangeSprint_Implementation(bool bOutSprint)
{
	if(bOutSprint)
	{
		StartSprint();
	}else
	{
		StopSprint();
	}
}

bool ALGCharacterBase::Server_ChangeSprint_Validate(bool bOutSprint)
{
	return true;
}

void ALGCharacterBase::Server_StopIronSight_Implementation()
{
	StopIronSight();
}

bool ALGCharacterBase::Server_StopIronSight_Validate()
{
	return true;
}

void ALGCharacterBase::Multi_Dead_Implementation(FVector Impulse, FName BoneName)
{
	//布娃娃模拟
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->AddImpulse(Impulse*2000,BoneName,true);//true表示作用于结果 忽略质量
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called every frame
void ALGCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALGCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ALGCharacterBase::IsSprinting()
{
	if(IsHoldWeapon())
	{
		return bSprint&&!bIsCrouched && FVector::DotProduct(GetVelocity().GetSafeNormal(),GetActorForwardVector())>0.9f;//值为1：两向量完全平行（同向）
		//值为0：两向量垂直
		//值为-1：两向量方向相反
	}
	else
	{
		return bSprint && !bIsCrouched;
	}
}

bool ALGCharacterBase::IsHoldWeapon()
{
	return PackageComponent && PackageComponent->GetHoldWeapon();
}

void ALGCharacterBase::SetLockView(bool bLock)
{
	bUseControllerRotationYaw=bLock;
	GetCharacterMovement()->bOrientRotationToMovement=!bLock;//是锁定状态界面的时候 角色旋转跟随控制器（相机 物理鼠标），不跟随角色移动方向（WASD）
}

AWeaponActor* ALGCharacterBase::GetHoldWeapon()
{
	return PackageComponent->GetHoldWeapon();
}

void ALGCharacterBase::StartFire()
{
	if(GetHoldWeapon() && !IsSprinting())
	{
		GetHoldWeapon()->StartFire();
	}
}

void ALGCharacterBase::StopFire()
{
	if(GetHoldWeapon())
	{
		GetHoldWeapon()->StopFire();
	}
}

void ALGCharacterBase::ReloadWeapon()
{
	if(GetHoldWeapon())
	{
		GetHoldWeapon()->ReloadWeapon();
	}
}

bool ALGCharacterBase::IsDead()
{
	return CurrentHP<=0;
}

