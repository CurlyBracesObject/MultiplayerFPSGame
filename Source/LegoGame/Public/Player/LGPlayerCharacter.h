// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LGCharacterBase.h"
#include "LGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
/**
 * 
 */
UCLASS()
class LEGOGAME_API ALGPlayerCharacter : public ALGCharacterBase
{
	GENERATED_BODY()
public:
	ALGPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	USpringArmComponent* GetSpringArmComponent(){return SpringArmComponent;}

	virtual UCameraComponent* GetCameraComponent() override {return CameraComponent;}
	
protected:

	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void DoJump();

	void TogglePackageUI();
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex);
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	
	
};
