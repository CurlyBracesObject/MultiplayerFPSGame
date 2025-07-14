// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/KeyBoardWidget.h"

#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"


void UKeyBoardWidget::InitKeyBoard(FText KeyBoardName, FKey Key,FName OutKeyEventName, float OutKeyScale)
{
	KeyStringTextBlock->SetText(KeyBoardName);
	KeySelector->SelectedKey=Key;
	CurrentKey=Key;
	//记录这块板子跟哪一个 用户输入的配置按键名称关联
	this->KeyEventName=OutKeyEventName;
	this->KeyScale=OutKeyScale;
}

void UKeyBoardWidget::SetInputKey(FKey NewKey)
{
	//键值更新到用户输入配置表里
	if(KeyScale==0)
	{
		FInputActionKeyMapping KeyMapping;
		KeyMapping.Key=CurrentKey;//这个是什么意思 直接UE项目里的keymapping设置被更改了吗？KeyMapping.Key这个直接引出来的是 直接更改UE项目中的按键映射设置
		KeyMapping.ActionName=KeyEventName;
		UInputSettings::GetInputSettings()->RemoveActionMapping(KeyMapping);
		KeyMapping.Key=NewKey;
		UInputSettings::GetInputSettings()->AddActionMapping(KeyMapping);
	}
	else//说明是轴事件
		{
		FInputAxisKeyMapping KeyMapping;
		KeyMapping.Key=CurrentKey;
		KeyMapping.Scale=KeyScale;
		KeyMapping.AxisName=KeyEventName;
		UInputSettings::GetInputSettings()->RemoveAxisMapping(KeyMapping);
		KeyMapping.Key=NewKey;
		UInputSettings::GetInputSettings()->AddAxisMapping(KeyMapping);
		}
	CurrentKey=NewKey;
}

void UKeyBoardWidget::SetKeySelector(FKey NewKey)
{
	KeySelector->SetSelectedKey(NewKey);
}

/*void UKeyBoardWidget::SetKeyEventInfo()
{
	this->KeyEventName=OutKeyEventName;
	this->KeyScale=OutKeyScale;
}*/

void UKeyBoardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//绑定事件通知
	KeySelector->OnKeySelected.AddDynamic(this,&UKeyBoardWidget::OnKeySelected);
			//SelectedKey代表的是用户刚刚在UI上选择的新按键，
	//这里的OnKeySelected是UE4 UI控件UInputKeySelector的事件，
	//当用户在界面上选择了一个新按键时触发，并将用户选择的按键信息作为FInputChord类型的参数传递给你的处理函数。
}

void UKeyBoardWidget::OnKeySelected(FInputChord SelectedKey)
{
	//UE_LOG(LogTemp, Log, TEXT("==%s"),*SelectedKey.Key.ToString());
	//使用*运算符将FString转换为TCHAR*    要TCHAR*
	if(CurrentKey==SelectedKey.Key)
	{
		return;
	}

		//查重
	auto ActionMappings=UInputSettings::GetInputSettings()->GetActionMappings();
	for(auto Action:ActionMappings)
	{
		if(Action.Key==SelectedKey.Key)
		{
			KeySelector->SetSelectedKey(CurrentKey);//用来拒绝用户的按键修改并恢复原来的按键设置。
			//将UI中的按键选择器控件   重置回原来的按键值（CurrentKey）
			return;
		}
	}

	auto AxisMappings=UInputSettings::GetInputSettings()->GetAxisMappings();
	for(auto Axis:AxisMappings)
	{
		if(Axis.Key==SelectedKey.Key)
		{
			KeySelector->SetSelectedKey(CurrentKey);
			return;
		}
	}


	SetInputKey(SelectedKey.Key);

}
