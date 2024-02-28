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
	virtual void Tick(const float DeltaTime) override; // Idle�� Aiming ������ ������ ���� �Լ�
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override; //Aiming�� replicated�ϱ� ���� �Լ�

public:	

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class USkeletalMeshComponent* ClientMesh; // ����� ĳ���Ͱ� ��ġ�� �ʱ� ���� �Ž�

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
	virtual void StartAiming(); //Aiming �ϴ� �Լ�

	UFUNCTION(BlueprintCallable,Category="Anim")
	virtual void ReverseAiming(); //�ڼ� ��ȯ �Լ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Anim")
	float ADSWeight=0.f; // ������ ������ ����

	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<UUserWidget> ZoomWidgetClass; // �������� �� UI Ŭ����
	UPROPERTY()
		UUserWidget* ZoomWidget; // // �������� �� UI

	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<UUserWidget> CharacterWidgetClass; // �������� �� UI Ŭ����
	UPROPERTY()
		UUserWidget* CharacterWidget; // // �������� �� UI

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Configurations|Anim")
	class UCurveFloat* AimingCurve; // Aiming�ϴ� �Ÿ�

	FTimeline AimingTimeline; //Aiming�ϴ� �ð�

	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bForward = true);
	virtual FORCEINLINE void Server_Aim_Implementation(const bool bForward) // ������ Aiming�� �ϴ� ���� ����
	{
		Multi_Aim(bForward);
		Multi_Aim_Implementation(bForward);
	} // �������� ������ �ʱ� ������ ���� Call �ؾ���.
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Aim(const bool bForward);
	virtual void Multi_Aim_Implementation(const bool bForward); // �ٸ� �ο��� ������ Aiming�� �ϰ� �ִٰ� ������

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
	bool FireEnd; // ���� �� ������
	float ReboundMovement = 0; // �󸶳� �ݵ��� �־�����
	float CurrentPitch; // �� �� ���� �� Pitch ��ġ
	FTimerHandle TimerHandle; // �� ������
};
