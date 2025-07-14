// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Widget/PackageScrollWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/PackageComponent.h"
#include "Components/ScrollBox.h"
#include "Player/LGPlayerCharacter.h"
#include "UMG/Widget/PackageBoardWidget.h"
#include "Scene/SceneItemActor.h"
#include "UMG/Widget/SkinFrameWidget.h"
#include "UMG/Widget/WeaponFrameWidget.h"

void UPackageScrollWidget::AddPackageBoard(ASceneItemActor* ItemActor)
{
	TSubclassOf<UPackageBoardWidget> BoardClass=LoadClass<UPackageBoardWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/UMG/Widget/Widget_PackageBoard.Widget_PackageBoard_C'"));
	UPackageBoardWidget* BoardWidget=CreateWidget<UPackageBoardWidget>(GetOwningPlayer(),BoardClass);
   if(BoardWidget)
   {
	   BoardWidget->InitPanel(ItemActor);
   	   MyScrollBox->AddChild(BoardWidget);
   }
}

void UPackageScrollWidget::RemovePackageBoard(ASceneItemActor* ItemActor)
{
	for(int32 i=0;i<MyScrollBox->GetChildrenCount();i++)
	{
		if(UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(MyScrollBox->GetChildAt(i)))
		{
			if(BoardWidget->GetSceneItemActor()==ItemActor)
			{
				BoardWidget->RemoveFromParent();
				break;
			}
		}
	}
}

void UPackageScrollWidget::AddPackageBoard(int32 Site, int32 ID)
{
	TSubclassOf<UPackageBoardWidget> BoardClass=LoadClass<UPackageBoardWidget>(nullptr,TEXT("WidgetBlueprint'/Game/Lego/UMG/Widget/Widget_PackageBoard.Widget_PackageBoard_C'"));
	UPackageBoardWidget* BoardWidget=CreateWidget<UPackageBoardWidget>(GetOwningPlayer(),BoardClass);
	if(BoardWidget)
	{
		BoardWidget->InitPanel(Site,ID);
		MyScrollBox->AddChild(BoardWidget);
	}
}

void UPackageScrollWidget::RemovePackageBoard(int32 Site)
{
	for(int32 i=0;i<MyScrollBox->GetChildrenCount();i++)
	{
		if(UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(MyScrollBox->GetChildAt(i)))
		{
			if(BoardWidget->GetPackageItemSite()==Site)
			{
				BoardWidget->RemoveFromParent();
				break;
			}
		}
	}
	
}

void UPackageScrollWidget::ClearScrollBox()
{
	MyScrollBox->ClearChildren();
}

bool UPackageScrollWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	if(!bPackageScrollBox){return false;}
	//都是向背包中添加道具
	if(UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(InOperation->Payload))//从附近过来的
	{															//InOperation->Payload 鼠标拖拽的那个UI
		//MyScrollBox->AddChild(BoardWidget);
		if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayerPawn()))
		{
			if(PlayerCharacter->GetPackageComponent())
			{
				PlayerCharacter->GetPackageComponent()->AddItemToPackage(BoardWidget->GetSceneItemActor());
			}//在MVC模式中，这个方法属于控制器(Controller)部分，它接收用户输入（拖放操作）并更新模型（调用AddItemToPackage）。
		}
		//上面的代码在干吗？
	}
	else if(USkinFrameWidget* SkinFrameWidget=Cast<USkinFrameWidget>(InOperation->Payload))
	{
		//说明在脱掉衣服
		if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayerPawn()))
		{
			if(PlayerCharacter->GetPackageComponent())
			{
				PlayerCharacter->GetPackageComponent()->TakeOffSkin(SkinFrameWidget->GetSkinType());//?
			}
		}
	}
	else if(UWeaponFrameWidget* WeaponFrameWidget=Cast<UWeaponFrameWidget>(InOperation->Payload))
	{
		if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayerPawn()))
		{
			if(PlayerCharacter->GetPackageComponent())
			{
				PlayerCharacter->GetPackageComponent()->UnEquipWeapon();
			}
		}
	}
	return true;
}
