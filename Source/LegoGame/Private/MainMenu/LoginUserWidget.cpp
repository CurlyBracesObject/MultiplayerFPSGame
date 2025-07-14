// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/LoginUserWidget.h"

#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "MainMenu/AccountSaveGame.h"
#include "MainMenu/MainMenuHUD.h"

void ULoginUserWidget::LoginAccount()
{
	if(SaveCheckBox->IsChecked())
	{
		//制作存档
		SaveData();
	}
	else
	{
		//删除账号密码存档
		UGameplayStatics::DeleteGameInSlot(TEXT("AccountData"),0);
	}
	RemoveFromParent();
	//关掉主菜单
	if(AMainMenuHUD* HUD=Cast<AMainMenuHUD>(GetOwningPlayer()->GetHUD()))
	{
		HUD->HideMainMenuUI();
		HUD->ShowRoomListUI();
	}
	
}

void ULoginUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//读取存档
	LoadData();
	if(AccountSaveGame)
	{
		SaveCheckBox->SetCheckedState(ECheckBoxState::Checked);
		AccountTextBox->SetText(FText::FromString(AccountSaveGame->Account));
		PasswordTextBox->SetText(FText::FromString(AccountSaveGame->Password));
	}
}

void ULoginUserWidget::SaveData()
{
	if(!AccountSaveGame)
	{
		AccountSaveGame=Cast<UAccountSaveGame>(UGameplayStatics::CreateSaveGameObject(UAccountSaveGame::StaticClass()));
	}
	AccountSaveGame->Account=AccountTextBox->GetText().ToString();
	AccountSaveGame->Password=PasswordTextBox->GetText().ToString();
	//存档
	UGameplayStatics::SaveGameToSlot(AccountSaveGame,TEXT("AccountData"),0);
}

void ULoginUserWidget::LoadData()
{
	//先检查是否存档
	if(UGameplayStatics::DoesSaveGameExist(TEXT("AccountData"),0))
	{
		AccountSaveGame=Cast<UAccountSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("AccountData"),0));
	}
}
