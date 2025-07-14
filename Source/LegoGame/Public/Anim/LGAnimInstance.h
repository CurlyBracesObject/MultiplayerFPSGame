// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LGAnimInstance.generated.h"

class ALGCharacterBase;
/**
 * 
 */
UCLASS()
class LEGOGAME_API ULGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	ULGAnimInstance(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	/*UFUNCTION()
	void AnimNotify_WeaponReload();*/
	UFUNCTION()
	void AnimNotify_RelaxedEnd();
	
protected:
	UPROPERTY(BlueprintReadOnly)
	float Speed;
	UPROPERTY(BlueprintReadOnly)
	uint8 bSprinting:1;//bSprinting 只占用 uint8 中的 1个位（bit）
	UPROPERTY(BlueprintReadOnly)
	uint8 bCrouch:1;
	UPROPERTY(BlueprintReadOnly)
	uint8 bHoldWeapon:1;
	UPROPERTY(BlueprintReadOnly)
	float Direction;
	UPROPERTY(BlueprintReadOnly)
	float AimPitch;
	UPROPERTY(BlueprintReadOnly)
	uint8 bIronSight:1;
	UPROPERTY(BlueprintReadOnly)
	FVector HoldWeaponPos;
	UPROPERTY(BlueprintReadOnly)
	uint8 bFire:1;
	UPROPERTY(BlueprintReadOnly)
	uint8 bJump:1;
	UPROPERTY(BlueprintReadOnly)
	uint8 bRelaxed:1;
	
	UPROPERTY()
	ALGCharacterBase* MyCharacter;

	float WaitRelaxed;
	
};
