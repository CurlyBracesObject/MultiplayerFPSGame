// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponFrameWidget.generated.h"

class UImage;
class AWeaponActor;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UWeaponFrameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateIcon(int32 ID);
	
protected:
	virtual void NativeConstruct() override;
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	void OnWeaponEquipped(AWeaponActor* Weapon);
	void OnWeaponUnEquipped(AWeaponActor* Weapon);
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;
};
