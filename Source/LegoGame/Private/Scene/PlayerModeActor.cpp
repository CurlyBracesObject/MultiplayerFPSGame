// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/PlayerModeActor.h"

#include "Components/SkinComponent.h"

// Sets default values
APlayerModeActor::APlayerModeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SkeletalMeshComponent=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMeshComp"));
	RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	SkinComponent=CreateDefaultSubobject<USkinComponent>(TEXT("SkinComp"));
}

// Called when the game starts or when spawned
void APlayerModeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

USkeletalMeshComponent* APlayerModeActor::GetSkinSkeletalMeshComponent()
{
	return SkeletalMeshComponent;
}

// Called every frame
void APlayerModeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

USkinComponent* APlayerModeActor::GetSkinComponent()
{
	/*if (!SkinComponent)
	{
		// 第一次调用时创建组件
		SkinComponent = NewObject<USkinComponent>(this);
		SkinComponent->RegisterComponent();
		UE_LOG(LogTemp, Warning, TEXT("动态创建了SkinComponent"));
	}*/
	return SkinComponent;
}

void APlayerModeActor::SetSkeletalMesh(USkeletalMesh* Mesh)
{
	SkeletalMeshComponent->SetSkeletalMesh(Mesh);
}

