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

	virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); } //시점 변화를 해당 블루프린트에 적용된 스켈레탈 메시 소켓의 Sights에 적용하기 위한 함수
};