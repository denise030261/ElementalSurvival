// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GunAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TRUEFPSTUTORIAL_API UGunAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	bool IsLeftMouseInputPressed();
	bool IsLeftMouseInputReleased();

public:
	UGunAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool IsShoot;
};
