// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystem/PropsSubsystem.h"
#include "SkinComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEGOGAME_API USkinComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkinComponent();

	void OnSkinPutOn(ESkinType Type,int32 ID);
	void OnSkinTakeOff(ESkinType Type,int32 ID);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UStaticMeshComponent* GetStaticMeshComponent(ESkinType Type);
	
	USkeletalMeshComponent* GetSkeletalMeshComponent();
	
	FName GetSocketName(ESkinType Type);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY()
	TMap<ESkinType,UStaticMeshComponent*> StaticMeshCompMap;//不同皮肤类型与对应的静态网格组件之间的映射关系
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;
};
