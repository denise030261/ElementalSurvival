// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapons/Weapon.h"
#include "TrueFPSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TRUEFPSTUTORIAL_API UTrueFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UTrueFPSAnimInstance();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION()
	virtual void CurrentWeaponChanged(class AWeapon* NewWeapon, const class AWeapon* OldWeapon);
	
	virtual void SetVars(const float DeltaTime);
	virtual void CalculateWeaponSway(const float DeltaTime);

	virtual void SetIKTransforms(); 

	bool IsSpaceInputPressed();
	bool IsRInputPressed();

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
	APlayerController* PlayerController;

public:
	UPROPERTY(BlueprintReadWrite, Category="Anim")
	class ATrueFPSCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	class AWeapon* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Anim")
	FIKProperties IKProperties;

	UPROPERTY(BlueprintReadOnly, Category="Anim")
	FRotator LastRotation; //��ȯ ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Anim")
	FTransform CameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform RelativeCameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform RHandToSightsTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform OffsetTransform; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float ADSWeight=0.f; // Idle�� Aiming�� ��ȣ�ۿ�
	
	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FRotator AccumulativeRotation; //��ȯ ����

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FRotator AccumulativeRotationInterp; //��ȯ ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool IsJumpAni = false; // �ȴ� �ӵ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	bool IsReload = false; // �ȴ� �ӵ�
};
