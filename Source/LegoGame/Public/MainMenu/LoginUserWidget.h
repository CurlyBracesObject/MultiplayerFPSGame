// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginUserWidget.generated.h"

class UEditableTextBox;
class UCheckBox;
class UAccountSaveGame;
/**
 * 
 */
UCLASS()
class LEGOGAME_API ULoginUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void LoginAccount();

	virtual void NativeOnInitialized() override;

	void SaveData();
	void LoadData();
protected:
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* AccountTextBox;
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* PasswordTextBox;
	UPROPERTY(meta=(BindWidget))
	UCheckBox* SaveCheckBox;

	UPROPERTY()
	UAccountSaveGame* AccountSaveGame;
};
