// Fill out your copyright notice in the Description page of Project Settings.


#include "Scene/DoorActor.h"
#include "Components/BoxComponent.h"
#include "Character/LGCharacterBase.h"

// Sets default values
ADoorActor::ADoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	StaticMeshComponent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	BoxComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAll"));
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this,&ADoorActor::OnSphereComponentBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this,&ADoorActor::OnSphereComponentEndOverlap);
	
	
}

void ADoorActor::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<ALGCharacterBase>(OtherActor))
	{
		bPlayAnim=true;
		CurveDirection=1;
	}

	OpenDir=FVector::DotProduct(GetActorRightVector(),(OtherActor->GetActorLocation()-GetActorLocation()).GetSafeNormal())<0?1:-1;//小于0钝角 玩家在门的左侧。门应该向右开
	//UE_LOG(LogTemp, Log, TEXT("进入范围 方向%s"),(CosValue>0?TEXT("在右侧"):TEXT("在左侧")));
}

void ADoorActor::OnSphereComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(Cast<ALGCharacterBase>(OtherActor))
	{
		bPlayAnim=true;
		CurveDirection=-1;
	}
}

void ADoorActor::UpdateAnim(float DeltaTime)
{
	if(!bPlayAnim || !DoorAnim)
	{//UE_LOG(LogTemp, Warning, TEXT("动画未播放（碰到和离开门都允许开始播放 “不应该播放”变量为true）或曲线为空"));
		return;
	}
	CurrentCurveTime+=(DeltaTime*CurveDirection);

	float Min,Max;
	DoorAnim->GetValueRange(Min,Max);//GetValueRange() 函数会修改这两个变量的值
	if(CurrentCurveTime<Min)
	{
		bPlayAnim=false;
		CurrentCurveTime=Min;
	}
	if(CurrentCurveTime>Max)
	{
		bPlayAnim=false;
		CurrentCurveTime=Max;
	}
	
	float Value = DoorAnim->GetFloatValue(CurrentCurveTime);
	StaticMeshComponent->SetRelativeRotation(FRotator(0,90*Value*OpenDir,0));
}

// Called every frame
void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAnim(DeltaTime);
}

