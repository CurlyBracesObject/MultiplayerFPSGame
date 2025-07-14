// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/SkinInterface.h"
#include "PlayerModeActor.generated.h"


class USkinComponent;
UCLASS()
class LEGOGAME_API APlayerModeActor : public AActor,public ISkinInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerModeActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual USkeletalMeshComponent* GetSkinSkeletalMeshComponent() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USkinComponent* GetSkinComponent();

	void SetSkeletalMesh(USkeletalMesh* Mesh);
protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY()
	USkinComponent* SkinComponent;

};
