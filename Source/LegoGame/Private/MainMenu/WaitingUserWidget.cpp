// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/WaitingUserWidget.h"
#include "Components/TextBlock.h"
void UWaitingUserWidget::ShowWaitingUI(int32 WaitTime)
{
	if(IsInViewport())
	{return;}
	CurrentWaitTime=WaitTime;
	if(WaitTime>0)
	{
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle,this,&UWaitingUserWidget::OnWaitTimeCallback,1);		
	}
	UpdateWaitTextBlock();
	AddToViewport();
}

void UWaitingUserWidget::OnWaitTimeCallback()
{
	if(--CurrentWaitTime>0)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle,this,&UWaitingUserWidget::OnWaitTimeCallback,1);
	}else
	{
		RemoveFromParent();
	}
	UpdateWaitTextBlock();
}

void UWaitingUserWidget::UpdateWaitTextBlock()
{
	FText WaitText;
	if(CurrentWaitTime==0)
	{
		WaitText=NSLOCTEXT("WaitingUI","k1","请等待");
	}
	else
	{
		WaitText=FText::Format(NSLOCTEXT("WaitingUI","k2","请等待({0})"),FText::AsNumber(CurrentWaitTime));
	}
	WaitTextBlock->SetText(WaitText);
}
