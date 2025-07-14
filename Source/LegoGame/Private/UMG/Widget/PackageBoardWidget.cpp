// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Widget/PackageBoardWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Scene/SceneItemActor.h"
#include "Subsystem/PropsSubsystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
void UPackageBoardWidget::InitPanel(ASceneItemActor* SceneItemActor)
{
	if(!SceneItemActor)
	{return;}
	const FPropsBase* PropsBase=GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(SceneItemActor->GetID());

	if(PropsBase)
	{
		//修改名字
		NameTextBlock->SetText(PropsBase->Name);
		//修改图标
		IconImage->SetBrushFromTexture(PropsBase->Icon);
	}
	MySceneItemActor=SceneItemActor;
}

void UPackageBoardWidget::InitPanel(int32 Site, int32 ID)
{
	PackageItemSite=Site;
     const FPropsBase* PropsBase=GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(ID);
     
     	if(PropsBase)
     	{
     		//修改名字
     		NameTextBlock->SetText(PropsBase->Name);
     		//修改图标
     		IconImage->SetBrushFromTexture(PropsBase->Icon);
     	}
}

FReply UPackageBoardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	//当用户按下左键并且移动鼠标一定距离时，会触发拖拽
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this,EKeys::LeftMouseButton).NativeReply;
}

void UPackageBoardWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	//创建拖拽代理
	OutOperation=UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());////创建拖拽操作对象
	//设置拖拽虚拟体
	OutOperation->DefaultDragVisual=this;//拖拽时显示的视觉元素就是当前控件本身
	//设置拖拽的关联对象
	OutOperation->Payload=this;//拖拽携带的数据是当前控件（放置时可以获取这个数据）
	//设置拖拽时鼠标的光标位置
	OutOperation->Pivot=EDragPivot::MouseDown;
	//Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}
