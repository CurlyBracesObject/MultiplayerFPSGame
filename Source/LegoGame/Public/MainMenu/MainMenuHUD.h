// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UOptionUserWidget;
class ULoginUserWidget;
class URoomListUserWidget;
class UMainMenuUserWidget;
class UWaitingUserWidget;

/**
 * 
 */
UCLASS()
class LEGOGAME_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ShowOptionUI();
	UFUNCTION(BlueprintCallable)
	void ShowLoginUI();
	UFUNCTION(BlueprintCallable)
	void ShowWaitingUI(int32 WaitTime);
	UFUNCTION(BlueprintCallable)
	void HideWaitingUI();
	

public:
	void ShowRoomListUI();

	void HideMainMenuUI();
	
protected:
	UPROPERTY()
	UOptionUserWidget* OptionUI;
	UPROPERTY()
	ULoginUserWidget* LoginUI;
	UPROPERTY()
	URoomListUserWidget* RoomListUI;
	UPROPERTY()
	UMainMenuUserWidget* MainMenuUI;
	UPROPERTY()
	UWaitingUserWidget* WaitingUI;
};
