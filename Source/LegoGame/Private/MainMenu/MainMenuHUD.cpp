// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuHUD.h"

#include "MainMenu/OptionUserWidget.h"
#include "MainMenu/LoginUserWidget.h"
#include "MainMenu/RoomListUserWidget.h"
#include "MainMenu/MainMenuUserWidget.h"
#include "MainMenu/WaitingUserWidget.h"


void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();
	if(!MainMenuUI)
	{
		TSubclassOf<UMainMenuUserWidget> MainMenuClass=LoadClass<UMainMenuUserWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/MainMenu/UMG/UMG_MainMenu.UMG_MainMenu_C'"));
		MainMenuUI=CreateWidget<UMainMenuUserWidget>(GetOwningPlayerController(),MainMenuClass);
	}
	if(MainMenuUI && !MainMenuUI->IsInViewport())
	{
		MainMenuUI->AddToViewport();
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AMainMenuHUD::ShowOptionUI()
{
	if(!OptionUI)
	{
		//加载类，然后创建userwidget
		TSubclassOf<UOptionUserWidget> OptionClass=LoadClass<UOptionUserWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/MainMenu/UMG/UMG_Option.UMG_Option_C'"));
		//加载并返回一个 TSubclassOf<UOptionUserWidget> 类型的对象，它本质上是一个指向类（Class）的引用，而不是实例（Instance）
		OptionUI=CreateWidget<UOptionUserWidget>(GetOwningPlayerController(),OptionClass);
	}
	if(OptionUI && !OptionUI->IsInViewport())
	{
		OptionUI->AddToViewport();
	}
}

void AMainMenuHUD::ShowLoginUI()
{
	if(!LoginUI)
	{
		TSubclassOf<ULoginUserWidget> LoginClass=LoadClass<ULoginUserWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/MainMenu/UMG/UMG_Login.UMG_Login_C'"));
		//加载并返回一个 TSubclassOf<UOptionUserWidget> 类型的对象，它本质上是一个指向类（Class）的引用，而不是实例（Instance）
		LoginUI=CreateWidget<ULoginUserWidget>(GetOwningPlayerController(),LoginClass);
	}
	if(LoginUI && !LoginUI->IsInViewport())
	{
		LoginUI->AddToViewport();
	}
}

void AMainMenuHUD::ShowWaitingUI(int32 WaitTime)
{
	if(!WaitingUI)
	{
		TSubclassOf<UWaitingUserWidget> WaitingClass=LoadClass<UWaitingUserWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/MainMenu/UMG/UMG_Waiting.UMG_Waiting_C'"));
		//加载并返回一个 TSubclassOf<UOptionUserWidget> 类型的对象，它本质上是一个指向类（Class）的引用，而不是实例（Instance）
		WaitingUI=CreateWidget<UWaitingUserWidget>(GetOwningPlayerController(),WaitingClass);
	}
	/*if(WaitingUI && !WaitingUI->IsInViewport())
	{
		WaitingUI->AddToViewport();
	}*/
	if(WaitingUI)
	{
		WaitingUI->ShowWaitingUI(WaitTime);
	}
}

void AMainMenuHUD::HideWaitingUI()
{
	if(WaitingUI)
	{
		WaitingUI->RemoveFromParent();
	}
}

void AMainMenuHUD::ShowRoomListUI()
{
	if(!RoomListUI)
	{
		TSubclassOf<URoomListUserWidget> RoomListClass=LoadClass<URoomListUserWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/MainMenu/UMG/UMG_RoomList.UMG_RoomList_C'"));
		//加载并返回一个 TSubclassOf<UOptionUserWidget> 类型的对象，它本质上是一个指向类（Class）的引用，而不是实例（Instance）
		RoomListUI=CreateWidget<URoomListUserWidget>(GetOwningPlayerController(),RoomListClass);
	}
	if(RoomListUI && !RoomListUI->IsInViewport())
	{
		RoomListUI->AddToViewport();
	}
}

void AMainMenuHUD::HideMainMenuUI()
{
	if(MainMenuUI)
	{
		MainMenuUI->RemoveFromParent();
	}
}
