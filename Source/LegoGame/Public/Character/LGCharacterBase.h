// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Interface/SkinInterface.h"
#include "LGCharacterBase.generated.h"

UENUM()
enum class ETeamColor
{
	ETC_Red,
	ETC_Blue,
	ETC_Yellow
};

class UPackageComponent;
class USkinComponent;
class AWeaponActor;
class ALGCharacterBase;
DECLARE_MULTICAST_DELEGATE_OneParam(NotifyDamage,ALGCharacterBase*);


UCLASS()
class LEGOGAME_API ALGCharacterBase : public ACharacter,public ISkinInterface,public IGenericTeamAgentInterface,public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALGCharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual class UCameraComponent* GetCameraComponent(){return nullptr;}//在基类中定义了一个虚函数

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartSprint();
	void StopSprint();
	void DoCrouch();
	
	virtual USkeletalMeshComponent* GetSkinSkeletalMeshComponent() override;

	void StartIronSight();
	void StopIronSight();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Net
	//
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_ChangeSprint(bool bOutSprint);
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_StartIronSight();
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_StopIronSight();
	UFUNCTION(NetMulticast,Reliable)
	void Multi_Dead(FVector Impulse,FName BoneName);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	bool IsSprinting();

	UPackageComponent* GetPackageComponent(){return PackageComponent;};
	USkinComponent* GetSkinComponent(){return SkinComponent;};
	bool IsHoldWeapon();

	void SetLockView(bool bLock);

	bool IsIronSight(){return bIronSight;};

	UFUNCTION(BlueprintCallable,BlueprintPure)
	AWeaponActor* GetHoldWeapon();

	UFUNCTION(BlueprintCallable)
	void StartFire();
	UFUNCTION(BlueprintCallable)
	void StopFire();
	
	void ReloadWeapon();
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool IsDead();
	
	//ETeamColor GetTeamColor()const{return TeamColor;}//AIController里OtherCharacter是常量 只能访问常函数
	
protected:
	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* BillboardComponent;

	UPROPERTY(Replicated)
	bool bSprint;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)//这两个有啥区别？为什么需要两个 一般是有什么用？
	UPackageComponent* PackageComponent;
	UPROPERTY()
	USkinComponent* SkinComponent;
	UPROPERTY(Replicated)
	bool bIronSight;

	UPROPERTY(EditAnywhere)
	ETeamColor TeamColor;

	UPROPERTY(EditAnywhere)
	int32 CurrentHP;
	UPROPERTY(EditAnywhere)
	int32 MaxHP;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer LGTagContainer;
	UPROPERTY(EditAnywhere)
	FGameplayTag LgGameplayTag;

public:
	NotifyDamage OnDamaged;//创建广播对象用于实现委托机制 允许其他组件或类注册监听器来响应TakeDamage  是一种观察者模式的实现
};
