// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "KeyMapWidget.generated.h"

struct FTableRowBase;
USTRUCT()
struct FWidgetKeyInfo:public FTableRowBase
{//FWidgetKeyInfo是个结构体 是个数据表格DT
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FName KeyEventName;
	UPROPERTY(EditAnywhere)
	FText KeyEventDescribe;
	UPROPERTY(EditAnywhere)
	FKey Key;
	UPROPERTY(EditAnywhere)
	float Scale;
};


class UVerticalBox;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UKeyMapWidget : public UUserWidget
{//KeyMapWidget是存放显示 键映射信息的UMG
	GENERATED_BODY()
protected:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION(BlueprintCallable)
	void ResetAllKey();
	
protected:
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* KeysVerticalBox;

	UPROPERTY()
	UDataTable* KeyInfoDT;
	
};
