// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RegisterUserWidget.generated.h"

class UButton;
class UTextBlock;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class LEGOGAME_API URegisterUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnSendButtonClicked();
	
	void LimitTimerCallBack();

	void UpdateButtonText();
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* SendCodeBtn;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* SendCodeBtnText;
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* MailTextBox;

	int32 LimitSendCodeBtn;
	
};
