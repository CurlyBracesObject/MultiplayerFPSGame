// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyBoardWidget.generated.h"

class UTextBlock;
class UInputKeySelector;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UKeyBoardWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitKeyBoard(FText KeyBoardName,FKey Key,FName OutKeyEventName, float OutKeyScale);
	//void SetKeyEventInfo(FName OutKeyEventName,float OutKeyScale);
	void SetInputKey(FKey NewKey);
	
	//重置后更新UI的方法
	void SetKeySelector(FKey NewKey);
	
	FName GetKeyEventName(){return KeyEventName;}
	float GetKeyScale(){return KeyScale;}
	FKey GetCurrentKey(){return CurrentKey;}
	
protected:
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void OnKeySelected(FInputChord SelectedKey);

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* KeyStringTextBlock;
	UPROPERTY(meta=(BindWidget))
	UInputKeySelector* KeySelector;

	FKey CurrentKey;

	FName KeyEventName;
	float KeyScale;
	
};
