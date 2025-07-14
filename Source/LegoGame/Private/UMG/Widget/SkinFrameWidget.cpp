// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Widget/SkinFrameWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/LGCharacterBase.h"
#include "Components/PackageComponent.h"
#include "Components/TextBlock.h"
#include "UMG/Widget/PackageBoardWidget.h"

#include "Components/Image.h"


void USkinFrameWidget::UpdateIcon(int32 ID)
{
	if(ID==-1)
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
		SkinSign->SetVisibility(ESlateVisibility::Visible);
	}else
	{
		IconImage->SetVisibility(ESlateVisibility::Visible);
		SkinSign->SetVisibility(ESlateVisibility::Hidden);

		const FPropsBase* PropsBase=GetOwningPlayer()->GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(ID);
		if(PropsBase)
		{
			IconImage->SetBrushFromTexture(PropsBase->Icon);
		}
	}
	
}

void USkinFrameWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(SkinSign)
	{
		FText SignText;
		if(SkinType==ESkinType::EST_Bag)
		{
			SignText=NSLOCTEXT("FramwWidget","k1","Bag");
		}else if(SkinType==ESkinType::EST_Beard)
		{
			SignText=NSLOCTEXT("FramwWidget","k2","Beard");
		}else if(SkinType==ESkinType::EST_Coat)
		{
			SignText=NSLOCTEXT("FramwWidget","k3","Coat");
		}
		else if(SkinType==ESkinType::EST_EyeMask)
		{
			SignText=NSLOCTEXT("FramwWidget","k4","EyeMask");
		}
		else if(SkinType==ESkinType::EST_Hair)
		{
			SignText=NSLOCTEXT("FramwWidget","k5","Hair");
		}
		else if(SkinType==ESkinType::EST_Helmmet)
		{
			SignText=NSLOCTEXT("FramwWidget","k6","Helmet");
		}
		else if(SkinType==ESkinType::EST_Glasses)
		{
			SignText=NSLOCTEXT("FramwWidget","k7","Glasses");
		}
		else if(SkinType==ESkinType::EST_FaceMask)
		{
			SignText=NSLOCTEXT("FramwWidget","k8","FaceMask");
		}
		else
		{
			SignText=NSLOCTEXT("FramwWidget","k12","Unknown");
		}

		SkinSign->SetText(SignText);
	}
}

bool USkinFrameWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if(UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(InOperation->Payload))
	{
		if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwningPlayerPawn()))
		{
			if(BoardWidget->GetSceneItemActor())
			{
				//从附近直接拖拽过来
			    CharacterBase->GetPackageComponent()->PutOnSkin(BoardWidget->GetSceneItemActor(),SkinType);
			}else
			{
				//从背包拖拽过来
				CharacterBase->GetPackageComponent()->PutOnSkin(BoardWidget->GetPackageItemSite(),SkinType);
			}
			return true;
		}
	}
	return false;
}

FReply USkinFrameWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(IconImage->IsVisible())
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this,EKeys::LeftMouseButton).NativeReply;
	}else
	{
		return FReply::Unhandled();//不去消耗点击事件 让它向下传递
	}
	
}

void USkinFrameWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	//Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	OutOperation=UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	OutOperation->Payload=this;
	OutOperation->DefaultDragVisual=this;//用户拖动此部件时，会看到部件本身作为视觉反馈被拖动，而不是一个占位符或其他视觉效果。
	OutOperation->Pivot=EDragPivot::MouseDown;//枢轴点(Pivot)为鼠标按下的位置(MouseDown)。//枢轴点决定了拖动视觉效果与鼠标指针的相对位置关系
	
}
