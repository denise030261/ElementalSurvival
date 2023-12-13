// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FIKProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimSequence* AnimPose; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AimOffset = 15.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform CustomOffsetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeightScale=1.f; // ���� ����, ��ȯ ����
};

UCLASS(Abstract)
class TRUEFPSTUTORIAL_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
		class ATrueFPSCharacter* CurrentOwner; // ���� ������ �ִ� ������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FIKProperties IKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform PlacementTransform;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
		FTransform GetSightsWorldTransform() const; //���� ��ȭ�� ���� �Լ��̴�.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* FireAnimation; // �� ��� �ִϸ��̼�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* ReloadGunAnim; // ���ε� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		USoundBase* ImpactSound; // �ε����� ���� �Ҹ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float MaxRange = 1000; // ���� �� �� �ִ� �ִ� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float ShootDelay = 0.1f; // �� ������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		int MaxBullet = 40; // �ִ� �Ѿ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int CurrentBullet = 40; // ���� �Ѿ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float Rebound = 0.05f; // �ݵ� ũ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	bool bShoot; // ���� �� �� �ִ���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float ReloadTime; // ���� �� �� �ִ���

	UPROPERTY()
	bool IsWeaponDelay; // ��� �������� �ð����� 

	UPROPERTY()
	float RemainingTime; // ��� �������� ���� �ð�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int32 GunKind;

	UFUNCTION()
		void Shooting(); // ���

	UFUNCTION()
		void StopShooting(); // ��� ����

	UFUNCTION()
		void Delaying(); // Delay�� ������ ���� �Լ�

	UFUNCTION()
		void Reloading(); // ������

	UFUNCTION()
		void ReloadDone(); // ������ �Ϸ�

	UFUNCTION()
		void TraceTarget(); //Ÿ���� ������ �Ҹ� �߻�

	UFUNCTION()
	AController* GetOwnerController() const; // Controller�ϰ� �ִ� ���� ��������

	virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); }
	//���� ��ȭ�� �ش� �������Ʈ�� ����� ���̷�Ż �޽� ������ Sights�� �����ϱ� ���� �Լ�

	bool GunTrace(FHitResult& Hit, FVector& ShotDirection); // Ÿ�� ����

private:
	FTimerHandle TimerHandle; // �����̸� ���� ����
	FTimerHandle ReloadTimerHandle; // ������ �����̸� ���� ����
};