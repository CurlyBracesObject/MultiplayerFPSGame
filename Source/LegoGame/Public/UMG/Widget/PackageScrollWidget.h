// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PackageScrollWidget.generated.h"

class UScrollBox;
class ASceneItemActor;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UPackageScrollWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddPackageBoard(ASceneItemActor* ItemActor);
	void RemovePackageBoard(ASceneItemActor* ItemActor);

	void AddPackageBoard(int32 Site,int32 ID);
	void RemovePackageBoard(int32 Site);
	
	
	void ClearScrollBox();
protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UScrollBox* MyScrollBox;
	UPROPERTY(EditAnywhere)
	uint8 bPackageScrollBox:1;
};
