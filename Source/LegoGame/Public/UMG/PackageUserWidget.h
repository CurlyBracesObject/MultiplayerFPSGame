// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystem/PropsSubsystem.h"
#include "Widget/PackageScrollWidget.h"
#include "Widget/WeaponFrameWidget.h"
#include "PackageUserWidget.generated.h"

class UVerticalBox;
class UPackageScrollWidget;
class APlayerModeActor;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UPackageUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder) override;

	virtual void RemoveFromParent() override;

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void OnNearItemEnter(ASceneItemActor* SceneItemActor);
	void OnNearItemLeave(ASceneItemActor* SceneItemActor);

	void OnPackageAddItem(int32 Site,int32 ID);//这个是监听函数
	void OnPackageRemoveItem(int32 Site,int32 ID);

	void OnSkinPutOn(ESkinType Type,int32 ID);
	void OnSkinTakeOff(ESkinType Type,int32 ID);

	void OnEquipWeapon(AWeaponActor* WeaponActor);
	void OnUnEquipWeapon(AWeaponActor* WeaponActor);

protected:
	UPROPERTY(meta=(BindWidget))//绑到编辑器里 
	UPackageScrollWidget* NearPackageScrollWidget;
	UPROPERTY(meta=(BindWidget))
	UPackageScrollWidget* PackageScrollWidget;
	
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* SkinVerticalBox;
	UPROPERTY()
	APlayerModeActor* PlayerModeActor;
	UPROPERTY(meta=(BindWidget))
	UWeaponFrameWidget* WeaponFrameWidget;
};
