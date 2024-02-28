// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "TrueFPSCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCurrentWeaponChangedDelegate, class AWeapon*, CurrentWeapon, const class AWeapon*, OldWeapon);


UCLASS()
class TRUEFPSTUTORIAL_API ATrueFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATrueFPSCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override; // Idle과 Aiming 사이의 과정을 위한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override; //Aiming을 replicated하기 위한 함수

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class USkeletalMeshComponent* ClientMesh; // 무기와 캐릭터가 겹치지 않기 위한 매시

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Configurations")
		TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
		TArray<class AWeapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon, Category = "State")
		class AWeapon* CurrentWeapon;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FCurrentWeaponChangedDelegate CurrentWeaponChangedDelegate; 

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
		int32 CurrentIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool IsJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool IsCrouch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool IsRun = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool IsShoot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float CurrentHP = 100;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int32 CurWeapon;

	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void EquipWeapon(const int32 Index);

protected:
	UFUNCTION()
		virtual void OnRep_CurrentWeapon(const class AWeapon* OldWeapon);

	UFUNCTION(Server, Reliable)
		void Server_SetCurrentWeapon(class AWeapon* Weapon);
		virtual void Server_SetCurrentWeapon_Implementation(class AWeapon* NewWeapon);

public:
	UFUNCTION(BlueprintCallable,Category="Anim")
	virtual void StartAiming(); //Aiming 하는 함수

	UFUNCTION(BlueprintCallable,Category="Anim")
	virtual void ReverseAiming(); //자세 변환 함수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Anim")
	float ADSWeight=0.f; // 서버에 보내는 변수

	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<UUserWidget> ZoomWidgetClass; // 스나이퍼 줌 UI 클래스
	UPROPERTY()
		UUserWidget* ZoomWidget; // // 스나이퍼 줌 UI

	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<UUserWidget> CharacterWidgetClass; // 스나이퍼 줌 UI 클래스
	UPROPERTY()
		UUserWidget* CharacterWidget; // // 스나이퍼 줌 UI

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Configurations|Anim")
	class UCurveFloat* AimingCurve; // Aiming하는 거리

	FTimeline AimingTimeline; //Aiming하는 시간

	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bForward = true);
	virtual FORCEINLINE void Server_Aim_Implementation(const bool bForward) // 서버에 Aiming을 하는 것을 제시
	{
		Multi_Aim(bForward);
		Multi_Aim_Implementation(bForward);
	} // 서버에서 해주지 않기 때문에 직접 Call 해야함.
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Aim(const bool bForward);
	virtual void Multi_Aim_Implementation(const bool bForward); // 다른 인원이 서버에 Aiming을 하고 있다고 제시함

	UFUNCTION()
	virtual void TimeLineProgress(const float Value); 
	
protected:
	virtual void NextWeapon();
	virtual void LastWeapon();

	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void LookUp(const float Value);
	void LookRight(const float Value);
	void Reloading();
	void Jumping();
	void Crouching();
	void Running();
	void StartShooting();
	void StopShooting();
	void CameraShake();

private:
	UPROPERTY()
		bool IsAim;
	bool FireEnd; // 총을 다 쐈는지
	float ReboundMovement = 0; // 얼마나 반동이 있었는지
	float CurrentPitch; // 총 다 쐈을 때 Pitch 위치
	FTimerHandle TimerHandle; // 총 딜레이
};
