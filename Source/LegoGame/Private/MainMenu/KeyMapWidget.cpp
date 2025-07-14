// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/KeyMapWidget.h"

#include "Components/VerticalBox.h"
#include "GameFramework/InputSettings.h"
#include "MainMenu/KeyBoardWidget.h"
//#include "Components/VerticalBox.h"

void UKeyMapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	TSubclassOf<UKeyBoardWidget> KeyBoardClass=LoadClass<UKeyBoardWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/MainMenu/UMG/Widget_KeyBoard.Widget_KeyBoard_C'"));

	//加载表格
	if(!KeyInfoDT)
	{
		KeyInfoDT=LoadObject<UDataTable>(this,TEXT("DataTable'/Game/Lego/Data/UI/DT_KeyInfo.DT_KeyInfo'"));
	}
	//下面两行代码是加载到内存里 (表格记录->加载内存->循环 因为有Action事件所以找对应的数据 
	TArray<FWidgetKeyInfo*> KeyInfos;
	KeyInfoDT->GetAllRows<FWidgetKeyInfo>(TEXT("解析数据表格错误！！KeyInfo表格"),KeyInfos);
	

	for(auto Info:KeyInfos)
	{
		UE_LOG(LogTemp, Log, TEXT("== %s"),*Info->KeyEventDescribe.ToString());
	}

	
	//配置显示Action操作按键信息  根据当前用户的按键配置更新按键面板
	const  TArray <FInputActionKeyMapping> KeyMappings=UInputSettings::GetInputSettings()->GetActionMappings();
	//UInputSettings::GetInputSettings()是获取全局输入设置实例的方法，             因为GetActionMappings()不是静态函数，所以必须先获取实例才能调用。

	const TArray<FInputAxisKeyMapping> AxisKeyMappings=UInputSettings::GetInputSettings()->GetAxisMappings();
	for(auto Info:KeyInfos)
	{
		for(auto Key:KeyMappings)
		{
			if(Info->KeyEventName==Key.ActionName)
			{
				UKeyBoardWidget* KeyBoardWidget=CreateWidget<UKeyBoardWidget>(GetOwningPlayer(),KeyBoardClass);
				KeyBoardWidget->InitKeyBoard(Info->KeyEventDescribe,Key.Key,Key.ActionName,0);
				KeysVerticalBox->AddChild(KeyBoardWidget);
				break;
			}
		}

	

		for(auto Axis:AxisKeyMappings)
		{
			if(Info->KeyEventName==Axis.AxisName &&Info->Scale==Axis.Scale)
			{
				UKeyBoardWidget* KeyBoardWidget=CreateWidget<UKeyBoardWidget>(GetOwningPlayer(),KeyBoardClass);
				KeyBoardWidget->InitKeyBoard(Info->KeyEventDescribe,Axis.Key,Axis.AxisName,Axis.Scale);
				KeysVerticalBox->AddChild(KeyBoardWidget);
				break;
			}
			
		}
	}
}



/*const  TArray <FInputActionKeyMapping> KeyMappings=UInputSettings::GetInputSettings()->GetActionMappings();
	 *for(auto Key:KeyMappings)
	{
		//UE_LOG(LogTemp, Log, TEXT("%s"),*Key.ActionName.ToString());
		UKeyBoardWidget* KeyBoardWidget=CreateWidget<UKeyBoardWidget>(GetOwningPlayer(),KeyBoardClass);

		//循环 因为有Action事件(UInputSettings::GetInputSettings()->GetActionMappings()
								//->CreateWidget<UKeyBoardWidget>(GetOwningPlayer(),KeyBoardClass);) 所以找对应的数据 
		for(auto Info:KeyInfos)
		{
			if(Info->KeyEventName==Key.ActionName)
			{
				//KeyBoardWidget->InitKeyBoard(FText::FromName(Key.ActionName),Key.Key);
				KeyBoardWidget->InitKeyBoard(Info->KeyEventDescribe,Key.Key,Key.ActionName,0);
				break;
			}
		}
		KeysVerticalBox->AddChild(KeyBoardWidget);
	}*/

	/*
	//配置显示轴事件按键信息
	const TArray<FInputAxisKeyMapping> AxisKeyMappings=UInputSettings::GetInputSettings()->GetAxisMappings();
	for(auto Axis:AxisKeyMappings)
	{
		UKeyBoardWidget* KeyBoardWidget=CreateWidget<UKeyBoardWidget>(GetOwningPlayer(),KeyBoardClass);
		for(auto Info:KeyInfos)
		{
			if(Info->KeyEventName==Axis.AxisName &&Info->Scale==Axis.Scale)
			{
				KeyBoardWidget->InitKeyBoard(Info->KeyEventDescribe,Axis.Key,Axis.AxisName,Axis.Scale);
				break;
			}
		}
		KeysVerticalBox->AddChild(KeyBoardWidget);
	}*/

void UKeyMapWidget::ResetAllKey()
{
	//加载表格
	if(!KeyInfoDT)
	{
		KeyInfoDT=LoadObject<UDataTable>(this,TEXT("DataTable'/Game/Lego/Data/UI/DT_KeyInfo.DT_KeyInfo'"));
	}
	//下面两行代码是加载到内存里 (表格记录->加载内存->循环 因为有Action事件所以找对应的数据 
	TArray<FWidgetKeyInfo*> KeyInfos;
	KeyInfoDT->GetAllRows<FWidgetKeyInfo>(TEXT("解析数据表格错误！！KeyInfo表格"),KeyInfos);

	//获取到所有的UI板子
	for(int32 i=0;i<KeysVerticalBox->GetChildrenCount();i++)
	{
		if(UKeyBoardWidget* KeyBoardWidget=Cast<UKeyBoardWidget>(KeysVerticalBox->GetChildAt(i)))
		{
			for(auto KeyInfo:KeyInfos)
			{
				if(KeyInfo->KeyEventName==KeyBoardWidget->GetKeyEventName()&&KeyInfo->Scale==KeyBoardWidget->GetKeyScale())
				{//当前控件中的按键 的信息： KeyBoardWidget->GetKeyEventName()  KeyBoardWidget->GetKeyScale()
					if(KeyInfo->Key !=KeyBoardWidget->GetCurrentKey())
					{
						//如果BT中的按键 和 当前控件中的按键 的信息不同
							//更新输入系统中的按键
					KeyBoardWidget->SetInputKey(KeyInfo->Key);
							//更新UI显示
					KeyBoardWidget->SetKeySelector(KeyInfo->Key);
					}
					break;
				}
			}
		}
	}
}