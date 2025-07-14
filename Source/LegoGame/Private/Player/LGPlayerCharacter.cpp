// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LGPlayerCharacter.h"

#include "LGHUD.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PackageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LGPlayerController.h"
#include "Subsystem/PropsSubsystem.h"
#include "Components/SphereComponent.h"
#include "LegoGame/LegoGame.h"
#include "Scene/SceneItemActor.h"


ALGPlayerCharacter::ALGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SpringArmComponent=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComponent=CreateDefaultSubobject<UCameraComponent>(TEXT("CamaraComp"));

	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);
	

	SpringArmComponent->bUsePawnControlRotation=true;

	SphereComponent=CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	SphereComponent->SetSphereRadius(150.f);
	/*//绑定通知
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this,&ALGPlayerCharacter::OnComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this,&ALGPlayerCharacter::OnComponentEndOverlap);*/
	
}

void ALGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	/*// 确保SphereComponent正确设置
	if(SphereComponent)
	{
		SphereComponent->SetGenerateOverlapEvents(true);
		//UE_LOG(LogTemp, Warning, TEXT("SphereComponent is ready for overlap detection"));
	}
	/*if(SphereComponent)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
		SphereComponent->SetGenerateOverlapEvents(true);
	}#1#
	//绑定通知
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this,&ALGPlayerCharacter::OnComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this,&ALGPlayerCharacter::OnComponentEndOverlap);*/


	if(SphereComponent)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->SetGenerateOverlapEvents(true);

		SphereComponent->SetCollisionProfileName(TEXT("OverlapAll")); 
		SphereComponent->SetCollisionResponseToChannel(ECC_SceneItem, ECR_Overlap);
		/*//创建碰撞响应容器
		FCollisionResponseContainer ResponseContainer;
		ResponseContainer.SetAllChannels(ECR_Overlap);
		SphereComponent->SetCollisionResponseToChannels(ResponseContainer);*/
		
		
		//UE_LOG(LogTemp, Warning, TEXT("SphereComponent半径: %f，碰撞设置已更新"), SphereComponent->GetScaledSphereRadius());
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this,&ALGPlayerCharacter::OnComponentBeginOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this,&ALGPlayerCharacter::OnComponentEndOverlap);
	}
}

void ALGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this,&ALGPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this,&ALGPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"),this,&ALGPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"),this,&ALGPlayerCharacter::LookUp);
	
	PlayerInputComponent->BindAction(TEXT("DoJump"),IE_Pressed,this,&ALGPlayerCharacter::DoJump);
	PlayerInputComponent->BindAction(TEXT("DoSprint"),IE_Pressed,this,&ALGPlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction(TEXT("DoSprint"),IE_Released,this,&ALGPlayerCharacter::StopSprint);
	PlayerInputComponent->BindAction(TEXT("DoCrouch"),IE_Pressed,this,&ALGPlayerCharacter::DoCrouch);
	PlayerInputComponent->BindAction(TEXT("TogglePackage"),IE_Pressed,this,&ALGPlayerCharacter::TogglePackageUI);
	PlayerInputComponent->BindAction(TEXT("ToggleIronSight"),IE_Pressed,this,&ALGPlayerCharacter::StartIronSight);
	PlayerInputComponent->BindAction(TEXT("ToggleIronSight"),IE_Released,this,&ALGPlayerCharacter::StopIronSight);

	PlayerInputComponent->BindAction(TEXT("DoFire"),IE_Pressed,this,&ALGPlayerCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("DoFire"),IE_Released,this,&ALGPlayerCharacter::StopFire);

	PlayerInputComponent->BindAction(TEXT("DoReload"),IE_Pressed,this,&ALGPlayerCharacter::ReloadWeapon);
}



void ALGPlayerCharacter::MoveForward(float Value)
{
	if(Value==0)return;
	//AddMovementInput(GetActorForwardVector(),Value);
	//AddMovementInput(GetControlRotation().Quaternion().GetAxisX(),Value);
	AddMovementInput(FRotator(0,GetControlRotation().Yaw,0).Quaternion().GetAxisX(),Value);
	                              //创建了一个旋转器   FRotator 没有直接获取轴向量的方法，而 FQuat（四元数）有
										//四元数确实还是 3D 的   但这个特定的四元数只代表水平面上的旋转
}

void ALGPlayerCharacter::MoveRight(float Value)
{
	if(Value==0)return;
	//AddMovementInput(GetActorRightVector(),Value);
	AddMovementInput(FRotator(0,GetControlRotation().Yaw,0).Quaternion().GetAxisY(),Value);
}

void ALGPlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value*GetWorld()->GetDeltaSeconds()*35);//每s要转的度数*帧和帧间隔的时间 <1的数
	//意思是把每s转的度数平均分配到 1s所有帧上，（所以能1s转相同度数 无论1s转多少帧）

	/*
	*（A：120FPS电脑：每次转动 = Value × 0.0083 × 35，一秒内总转动 ≈ 35度
B：30FPS电脑：每次转动 = Value × 0.033 × 35，一秒内总转动 ≈ 35度）

120FPS，1秒120帧，Value在两台电脑上值是一样的
这个函数是每帧执行一次
所以1s内调用这个函数的次数是不同的，
A电脑走1/120Value，1s内走这个函数120次；B电脑走1/30Value,1s内走这个函数30次，所以1s内走的度数结果会是一样的 
	 * 
	 */
}

void ALGPlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value*GetWorld()->GetDeltaSeconds()*35);
}

void ALGPlayerCharacter::DoJump()
{
	Super::Jump();
	//GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->SayHello();//永远都是那一份 做成了单例 啥意思？
}

void ALGPlayerCharacter::TogglePackageUI()
{
	if(APlayerController* PC=Cast<APlayerController>(GetController()))
	{
		if(ALGHUD* HUD=Cast<ALGHUD>(PC->GetHUD()))
		{
			HUD->TogglePackage();
		}
	}
}

void ALGPlayerCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if(ASceneItemActor* ItemActor=Cast<ASceneItemActor>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Begin Overlap with %s"),*OtherActor->GetName());
	}*/
	//UE_LOG(LogTemp, Warning, TEXT("Begin Overlap with actor: %s"),*OtherActor->GetName());
	if(ASceneItemActor* SceneItemActor=Cast<ASceneItemActor>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("It's a SceneItemActor!begin"));
		if(PackageComponent)
		{
			PackageComponent->AddNearSceneItems(SceneItemActor);
		}
	}
	
}

void ALGPlayerCharacter::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Log, TEXT("End"));
	if(ASceneItemActor* SceneItemActor=Cast<ASceneItemActor>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("It's a SceneItemActor!end"));
		if(PackageComponent)
		{
			PackageComponent->RemoveNearSceneItems(SceneItemActor);
		}
	}
}





