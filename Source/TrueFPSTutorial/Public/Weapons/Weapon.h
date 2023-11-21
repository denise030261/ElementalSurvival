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
	float WeightScale=1.f; // 무기 무게, 변환 이후
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
		class ATrueFPSCharacter* CurrentOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FIKProperties IKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform PlacementTransform;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
		FTransform GetSightsWorldTransform() const; //시점 변화를 위한 함수이다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* ReloadGunAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float MaxRange = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float ShootDelay = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float Rebound = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		int MaxBullet = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int CurrentBullet = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	bool bShoot;

	UFUNCTION()
	void StartShooting();

	UFUNCTION()
	void Shooting();

	UFUNCTION()
	void StopShooting();

	UFUNCTION()
		void Reloading();

	UFUNCTION()
	AController* GetOwnerController() const;

	virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); }
	//시점 변화를 해당 블루프린트에 적용된 스켈레탈 메시 소켓의 Sights에 적용하기 위한 함수

	bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

private:
	FTimerHandle TimerHandle;
};