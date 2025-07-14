// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UENUM()
enum class EWeaponState:uint8
{
	EWS_Normal,
	EWS_Fire,
	EWS_Reload,
	EWS_Empty
};


class ALGCharacterBase;
class USoundCue;


UCLASS()
class LEGOGAME_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnBullet();
	
	void SpawnEffect();
	
	void SpawnDamage();
	
	void GetFirePostAndDirection(FVector& Position,FVector& Direction);//FVector&?

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentClipVolume();

	//Net
	UFUNCTION(NetMulticast,Reliable)
	void NetMulti_PlayReloadAnim();

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_StartFire();
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_StopFire();
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_Reload();
	
public:	
	void StartFire();
	void StopFire();

	void SetMaster(ALGCharacterBase* CharacterBase);

	void SetID(int32 OutID){ID=OutID;}
	int32 GetID(){return ID;}

	USkeletalMeshComponent* GetSkeletalMeshComponent(){return SkeletalMeshComponent;}

	UFUNCTION(BlueprintCallable,BlueprintPure)//不会修改对象的状态 只是获取/返回数据
	int32 GetCurrentClipVolume(){return  CurrentClipVolume;}
	int32 GetMaxClipVolume(){return MaxClipVolume;}

	float ReloadWeapon();
	
	void MakeFullClip();

	EWeaponState GetCurrentState();
	
protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(Replicated)//里面不用填啥吗
	ALGCharacterBase* MyMaster;
	
	int32 ID;
	
	UPROPERTY(EditAnywhere)
	int32 MaxClipVolume;
	UPROPERTY(EditAnywhere)
	float FireInterval;
	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* FirePartical;
	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentClipVolume)
	int32 CurrentClipVolume;

	FTimerHandle FireTimeHandle;
	
	float LastFireTime;
	UPROPERTY(Replicated)
	EWeaponState CurrentState;
};
