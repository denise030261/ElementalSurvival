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
		class ATrueFPSCharacter* CurrentOwner; // 총을 가지고 있는 소유자

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FIKProperties IKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform PlacementTransform;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
		FTransform GetSightsWorldTransform() const; //시점 변화를 위한 함수이다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* FireAnimation; // 총 사격 애니메이션

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset* ReloadGunAnim; // 리로드 에임

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		USoundBase* ImpactSound; // 부딪히면 나는 소리

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float MaxRange = 1000; // 총이 쏠 수 있는 최대 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		float ShootDelay = 0.1f; // 총 딜레이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
		int MaxBullet = 40; // 최대 총알

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int CurrentBullet = 40; // 현재 총알

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float Rebound = 0.05f; // 반동 크기

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	bool bShoot; // 총을 쏠 수 있는지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	float ReloadTime; // 총을 쏠 수 있는지

	UPROPERTY()
	bool IsWeaponDelay; // 사격 딜레이의 시간인지 

	UPROPERTY()
	float RemainingTime; // 사격 딜레이의 남은 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot")
	int32 GunKind;

	UFUNCTION()
		void Shooting(); // 사격

	UFUNCTION()
		void StopShooting(); // 사격 중지

	UFUNCTION()
		void Delaying(); // Delay의 간격을 위한 함수

	UFUNCTION()
		void Reloading(); // 재장전

	UFUNCTION()
		void ReloadDone(); // 재장전 완료

	UFUNCTION()
		void TraceTarget(); //타켓이 있으면 소리 발생

	UFUNCTION()
	AController* GetOwnerController() const; // Controller하고 있는 변수 가져오기

	virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); }
	//시점 변화를 해당 블루프린트에 적용된 스켈레탈 메시 소켓의 Sights에 적용하기 위한 함수

	bool GunTrace(FHitResult& Hit, FVector& ShotDirection); // 타켓 유무

private:
	FTimerHandle TimerHandle; // 딜레이를 위한 변수
	FTimerHandle ReloadTimerHandle; // 재장전 딜레이를 위한 변수
};