// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/RegisterUserWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void URegisterUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SendCodeBtn->OnClicked.AddDynamic(this,&URegisterUserWidget::OnSendButtonClicked);
}

void URegisterUserWidget::OnSendButtonClicked()
{
	//邮箱地址检查 是否合规
	FRegexPattern Pattern(TEXT("^\\w{1,10}@\\w{1,6}\\.\\w{2,6}$"));
	FRegexMatcher Matcher(Pattern,*MailTextBox->GetText().ToString());

	if(Matcher.FindNext())
	{
		UE_LOG(LogTemp, Log, TEXT("符合！"))
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("不符合！"))
	}
	
	LimitSendCodeBtn=3;
	//设置按钮不可用
	SendCodeBtn->SetIsEnabled(false);
	//启动定时器
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,this,&URegisterUserWidget::LimitTimerCallBack,1);
	UpdateButtonText();
}

void URegisterUserWidget::LimitTimerCallBack()
{
	if(--LimitSendCodeBtn>0)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle,this,&URegisterUserWidget::LimitTimerCallBack,1);
	}
	else
	{
		SendCodeBtn->SetIsEnabled(true);
	}
	UpdateButtonText();
}

void URegisterUserWidget::UpdateButtonText()
{
	FText SendCodeButtonText;
	if(LimitSendCodeBtn>0)
	{
		SendCodeButtonText=FText::Format(NSLOCTEXT("RegistUI","k2","发送({0})"),FText::AsNumber(LimitSendCodeBtn));
	}
	else
	{
		SendCodeButtonText=NSLOCTEXT("RegistUI","k2","发送");
	}
	SendCodeBtnText->SetText(SendCodeButtonText);
}
