// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneItemActor.generated.h"

UCLASS()
class LEGOGAME_API ASceneItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitActor();

	UStaticMeshComponent* GetStaticMeshComponent();
	USkeletalMeshComponent* GetSkeletalMeshComponent();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetID(){return ID;};
	void SetID(int32 OutID){ID=OutID;};
	

protected:
	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* BillboardComponent;
	UPROPERTY(EditAnywhere)
	int32 ID;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;
};
