// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify_ReloadWeapon.h"

#include "Character/LGCharacterBase.h"
#include "Weapon/WeaponActor.h"

void UAnimNotify_ReloadWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(MeshComp->GetOwner()))
	{
		CharacterBase->GetHoldWeapon()->MakeFullClip();
	}
}
