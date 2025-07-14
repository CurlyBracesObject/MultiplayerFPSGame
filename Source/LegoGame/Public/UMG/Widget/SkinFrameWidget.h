// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystem/PropsSubsystem.h"
#include "SkinFrameWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class LEGOGAME_API USkinFrameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	ESkinType GetSkinType(){return SkinType;};

	void UpdateIcon(int32 ID);
	
protected:
	virtual void NativePreConstruct() override;
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

protected:
	UPROPERTY(EditAnywhere)
	ESkinType SkinType;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SkinSign;
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;
	
};
