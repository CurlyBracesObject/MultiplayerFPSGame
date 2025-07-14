// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/PackageUserWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/PackageComponent.h"
#include "Components/SkinComponent.h"
#include "Player/LGPlayerCharacter.h"
#include "UMG/Widget/PackageBoardWidget.h"

#include "Components/VerticalBox.h"
#include "UMG/Widget/SkinFrameWidget.h"
#include "Scene/PlayerModeActor.h"
#include "Weapon/WeaponActor.h"


void UPackageUserWidget::AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder)
{
	Super::AddToScreen(LocalPlayer, ZOrder);

	//找到玩家的背包组件
	if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayer()->GetPawn()))
	{
		for(auto Item : PlayerCharacter->GetPackageComponent()->GetNearSceneItems())
		{
			NearPackageScrollWidget->AddPackageBoard(Item);
		}
		//绑定通知
		PlayerCharacter->GetPackageComponent()->OnNearSceneItemEnter.AddUObject(this,&UPackageUserWidget::OnNearItemEnter);//监听 谁想听这个广播，就把自己“挂在频道上”
		PlayerCharacter->GetPackageComponent()->OnNearSceneItemLeave.AddUObject(this,&UPackageUserWidget::OnNearItemLeave);

		PlayerCharacter->GetPackageComponent()->OnPackageItemAdded.AddUObject(this,&UPackageUserWidget::OnPackageAddItem);
		PlayerCharacter->GetPackageComponent()->OnPackageItemRemoved.AddUObject(this,&UPackageUserWidget::OnPackageRemoveItem);

		PlayerCharacter->GetPackageComponent()->OnSkinPutOn.AddUObject(this,&UPackageUserWidget::OnSkinPutOn);
		PlayerCharacter->GetPackageComponent()->OnSkinTakeOff.AddUObject(this,&UPackageUserWidget::OnSkinTakeOff);

		PlayerCharacter->GetPackageComponent()->OnEquipWeapon.AddUObject(this,&UPackageUserWidget::OnEquipWeapon);
		PlayerCharacter->GetPackageComponent()->OnUnEquipWeapon.AddUObject(this,&UPackageUserWidget::OnUnEquipWeapon);

		//下面是PlayerModeActor在场景中显示的逻辑：
		//将展示角色生成，并添加到场景
		if(!PlayerModeActor)
		{
			TSubclassOf<APlayerModeActor> PlayerModeClass=LoadClass<APlayerModeActor>(nullptr,TEXT("Blueprint'/Game/Lego/BP/Scene/BP_PlayerMode.BP_PlayerMode_C'"));
			PlayerModeActor=GetOwningPlayer()->GetWorld()->SpawnActor<APlayerModeActor>(PlayerModeClass,FVector(0,0,1000),FRotator::ZeroRotator);
			PlayerModeActor->SetSkeletalMesh(PlayerCharacter->GetMesh()->SkeletalMesh);
		}
		PlayerCharacter->GetPackageComponent()->OnSkinPutOn.AddUObject(PlayerModeActor->GetSkinComponent(),&USkinComponent::OnSkinPutOn);
		PlayerCharacter->GetPackageComponent()->OnSkinTakeOff.AddUObject(PlayerModeActor->GetSkinComponent(),&USkinComponent::OnSkinTakeOff);
		PlayerModeActor->SetActorHiddenInGame(false);
	}
	/*UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(GetWidgetFromName(TEXT("a")));
	BoardWidget->RemoveFromParent();*/
}

void UPackageUserWidget::RemoveFromParent()
{
	Super::RemoveFromParent();
	if(NearPackageScrollWidget)
	{
		NearPackageScrollWidget->ClearScrollBox();
	}
	if(GetOwningPlayer())
	{	if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayer()->GetPawn()))
     	{
     		PlayerCharacter->GetPackageComponent()->OnNearSceneItemEnter.RemoveAll(this);
     		PlayerCharacter->GetPackageComponent()->OnNearSceneItemLeave.RemoveAll(this);

			PlayerCharacter->GetPackageComponent()->OnPackageItemAdded.RemoveAll(this);
			PlayerCharacter->GetPackageComponent()->OnPackageItemRemoved.RemoveAll(this);

			PlayerCharacter->GetPackageComponent()->OnSkinPutOn.RemoveAll(this);
			PlayerCharacter->GetPackageComponent()->OnSkinTakeOff.RemoveAll(this);

			PlayerModeActor->SetActorHiddenInGame(true);
			PlayerCharacter->GetPackageComponent()->OnSkinPutOn.RemoveAll(PlayerModeActor->GetSkinComponent());
			PlayerCharacter->GetPackageComponent()->OnSkinTakeOff.RemoveAll(PlayerModeActor->GetSkinComponent());

			PlayerCharacter->GetPackageComponent()->OnEquipWeapon.RemoveAll(this);
			PlayerCharacter->GetPackageComponent()->OnUnEquipWeapon.RemoveAll(this);
     	}
	}
}


bool UPackageUserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if(UPackageBoardWidget* BoardWidget=Cast<UPackageBoardWidget>(InOperation->Payload))
	{
		if(!BoardWidget->GetSceneItemActor())//说明你拖拽松手的面板不是关联的场景的道具
		{
			if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayerPawn()))
			{
				if(PlayerCharacter->GetPackageComponent())
				{
					PlayerCharacter->GetPackageComponent()->RemoveItemFromPackage(BoardWidget->GetPackageItemSite());
					return true;
				}
			}
		}
	}
	else if(USkinFrameWidget* SkinFrameWidget=Cast<USkinFrameWidget>(InOperation->Payload))
	{
		if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayerPawn()))
		{
			if(PlayerCharacter->GetPackageComponent())
			{
				PlayerCharacter->GetPackageComponent()->TakeOffSkinToScreen(SkinFrameWidget->GetSkinType());
			}
		}
	}
	else if(UWeaponFrameWidget* WeaponWidget=Cast<UWeaponFrameWidget>(InOperation->Payload))
	{
		if(ALGPlayerCharacter* PlayerCharacter=Cast<ALGPlayerCharacter>(GetOwningPlayerPawn()))
		{
			if(PlayerCharacter->GetPackageComponent())
			{
				PlayerCharacter->GetPackageComponent()->UnEquipWeapon(false);
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UPackageUserWidget::OnNearItemEnter(ASceneItemActor* SceneItemActor)
{
	NearPackageScrollWidget->AddPackageBoard(SceneItemActor);
}

void UPackageUserWidget::OnNearItemLeave(ASceneItemActor* SceneItemActor)
{
	NearPackageScrollWidget->RemovePackageBoard(SceneItemActor);
}

void UPackageUserWidget::OnPackageAddItem(int32 Site, int32 ID)//一个是在背包里的位置  一个自己的id 
{
	PackageScrollWidget->AddPackageBoard(Site,ID);
}

void UPackageUserWidget::OnPackageRemoveItem(int32 Site, int32 ID)//所以这个方法一个参数就行了？
{
	PackageScrollWidget->RemovePackageBoard(Site);
}

void UPackageUserWidget::OnSkinPutOn(ESkinType Type, int32 ID)
{
	for(int32 i=0;i<SkinVerticalBox->GetChildrenCount();i++)
	{
		if(USkinFrameWidget* FrameWidget=Cast<USkinFrameWidget>(SkinVerticalBox->GetChildAt(i)))
		{
			if(FrameWidget->GetSkinType()==Type)
			{
				FrameWidget->UpdateIcon(ID);
				break;
			}
		}
	}
}

void UPackageUserWidget::OnSkinTakeOff(ESkinType Type, int32 ID)
{
	for(int32 i=0;i<SkinVerticalBox->GetChildrenCount();i++)
	{
		if(USkinFrameWidget* FrameWidget=Cast<USkinFrameWidget>(SkinVerticalBox->GetChildAt(i)))
		{
			if(FrameWidget->GetSkinType()==Type)
			{
				FrameWidget->UpdateIcon(-1);
				break;
			}
		}
	}
}

void UPackageUserWidget::OnEquipWeapon(AWeaponActor* WeaponActor)
{
	WeaponFrameWidget->UpdateIcon(WeaponActor->GetID());
}

void UPackageUserWidget::OnUnEquipWeapon(AWeaponActor* WeaponActor)
{
	WeaponFrameWidget->UpdateIcon(-1);
}

