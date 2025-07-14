// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Widget/WeaponFrameWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/LGCharacterBase.h"
#include "Components/PackageComponent.h"
#include "UMG/Widget/PackageBoardWidget.h"

#include "Components/Image.h"
#include "Weapon/WeaponActor.h"

void UWeaponFrameWidget::UpdateIcon(int32 ID)
{
	if(ID==-1)
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
	}else
	{
		IconImage->SetVisibility(ESlateVisibility::Visible);

		const FPropsBase* PropsBase=GetOwningPlayer()->GetWorld()->GetGameInstance()->GetSubsystem<UPropsSubsystem>()->GetPropsBaseByID(ID);
		if(PropsBase)
		{
			//UE_LOG(LogTemp, Log, TEXT("FoundPropsBase,Setting texture"));
			IconImage->SetBrushFromTexture(PropsBase->Icon);
		}else
		{
			UE_LOG(LogTemp, Log, TEXT("PropsBase not found for ID:%d"),ID);
		}
	}
}

void UWeaponFrameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//绑定委托
	if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwningPlayerPawn()))
	{
		if(UPackageComponent* PackageComponent=CharacterBase->GetPackageComponent())
		{
			PackageComponent->OnEquipWeapon.AddUObject(this,&UWeaponFrameWidget::OnWeaponEquipped);
			PackageComponent->OnUnEquipWeapon.AddUObject(this,&UWeaponFrameWidget::OnWeaponUnEquipped);
		}
	}
}

bool UWeaponFrameWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                      UDragDropOperation* InOperation)
{
	if(UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(InOperation->Payload))
	{
		if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwningPlayerPawn()))
		{
			if (BoardWidget->GetSceneItemActor()) //附近过来的穿戴
			{
				CharacterBase->GetPackageComponent()->EquipWeapon(BoardWidget->GetSceneItemActor());
			}
			else //背包过来的穿戴
			{
				CharacterBase->GetPackageComponent()->EquipWeapon(BoardWidget->GetPackageItemSite());
			}
			return true;//消耗这次的拖拽松起 这个为什么加这个if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwningPlayerPawn()))里面
		}

	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

FReply UWeaponFrameWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this,EKeys::LeftMouseButton).NativeReply;
}

void UWeaponFrameWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	OutOperation=UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	OutOperation->Payload=this;
	OutOperation->DefaultDragVisual=this;
	OutOperation->Pivot=EDragPivot::MouseDown;
}

void UWeaponFrameWidget::OnWeaponEquipped(AWeaponActor* Weapon)
{
	if(Weapon)
	{
		int32 WeaponID=Weapon->GetID();
		UE_LOG(LogTemp, Log, TEXT("UI received WeaponID: %d"), WeaponID);
		//UpdateIcon(Weapon->GetID());

		if(ALGCharacterBase* CharacterBase=Cast<ALGCharacterBase>(GetOwningPlayerPawn()))
		{
			if(UPackageComponent* PackageComponent=CharacterBase->GetPackageComponent())
			{
				UE_LOG(LogTemp, Log, TEXT("UI用PackageComponent->GetCurrentWeaponID()设置ID"))
				UpdateIcon(PackageComponent->GetCurrentWeaponID());
			}
		}
	}
}

void UWeaponFrameWidget::OnWeaponUnEquipped(AWeaponActor* Weapon)
{
	UpdateIcon(-1);//-1表示隐藏图标
}


