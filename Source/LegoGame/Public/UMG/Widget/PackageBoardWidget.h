// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

class ASceneItemActor;
class UTextBlock;
class UImage;

#include "PackageBoardWidget.generated.h"


/**
 * 
 */
UCLASS()
class LEGOGAME_API UPackageBoardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitPanel(ASceneItemActor* SceneItemActor);
	void InitPanel(int32 Site,int32 ID);

	ASceneItemActor* GetSceneItemActor(){return MySceneItemActor;};
	int32 GetPackageItemSite(){return PackageItemSite;};
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* NameTextBlock;
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;
	UPROPERTY()
	ASceneItemActor* MySceneItemActor;

	int32 PackageItemSite;
};
