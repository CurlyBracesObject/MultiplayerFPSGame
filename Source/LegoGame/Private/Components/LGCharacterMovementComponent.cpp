// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LGCharacterMovementComponent.h"

#include "Character/LGCharacterBase.h"

ULGCharacterMovementComponent::ULGCharacterMovementComponent()
{
	SprintSpeed=1000;
}

float ULGCharacterMovementComponent::GetMaxSpeed() const
{
	float Speed=Super::GetMaxSpeed();
	//如果当前使用组件的人，启用冲刺，我们更换冲刺速度
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwner()))
	{
		if(CharacterBase->IsSprinting())
		{
			Speed=SprintSpeed;
		}
	}
	return Speed;
}
