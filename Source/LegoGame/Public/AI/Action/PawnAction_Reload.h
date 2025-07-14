 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/PawnAction.h"
#include "PawnAction_Reload.generated.h"

/**
 * 
 */
UCLASS()
class LEGOGAME_API UPawnAction_Reload : public UPawnAction
{
	GENERATED_BODY()
public:
	UPawnAction_Reload();
protected:
	virtual bool Start() override;
	//virtual void Tick(float DeltaTime) override;
	//virtual void Finish(TEnumAsByte<EPawnActionResult::Type> WithResult) override;

	void ReloadEndCallBack();
protected:
	UPROPERTY(EditAnywhere)
	bool bBlockTask;

	
};
