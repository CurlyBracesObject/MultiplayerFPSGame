// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingUserWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class LEGOGAME_API UWaitingUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowWaitingUI(int32 WaitTime=0);

protected:
	void OnWaitTimeCallback();
	void UpdateWaitTextBlock();

protected:
	int32 CurrentWaitTime;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WaitTextBlock;

	
};
