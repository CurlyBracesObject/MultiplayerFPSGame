// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SearchNear.generated.h"

class AWeaponActor;
class ALGCharacterBase;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UBTService_SearchNear : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_SearchNear();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnInstanceCreated(UBehaviorTreeComponent& OwnerComp) override;

	void OnEquipWeapon(AWeaponActor* WeaponActor);

	void OnDamaged(ALGCharacterBase* CharacterBase);//注册进LGCharacterBase类中定义的OnDamaged广播对象（多播委托）的
	
protected:
	UPROPERTY(EditAnywhere)
	float SearchRadius;
	UPROPERTY()
	ALGCharacterBase* MyCharacterBase;
};
