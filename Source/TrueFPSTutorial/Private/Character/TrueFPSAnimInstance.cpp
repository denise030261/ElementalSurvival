// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TrueFPSAnimInstance.h"

#include "Camera/CAmeraComponent.h"
#include "Character/TrueFPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UTrueFPSAnimInstance::UTrueFPSAnimInstance()
{
}

void UTrueFPSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UTrueFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(!Character)
	{
		Character = Cast<ATrueFPSCharacter>(TryGetPawnOwner());
		if(Character)
		{
			Mesh = Character->GetMesh();
			Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UTrueFPSAnimInstance::CurrentWeaponChanged); 
			CurrentWeaponChanged(Character->CurrentWeapon, nullptr); 
		} 
		else return;
	}

	SetVars(DeltaTime);
	CalculateWeaponSway(DeltaTime);

	LastRotation = CameraTransform.Rotator(); //변환 이후
}

void UTrueFPSAnimInstance::CurrentWeaponChanged(AWeapon* NewWeapon, const AWeapon* OldWeapon)
{
	CurrentWeapon=NewWeapon;
	if(CurrentWeapon)
	{
		IKProperties = CurrentWeapon->IKProperties;
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UTrueFPSAnimInstance::SetIKTransforms);
	}
}

void UTrueFPSAnimInstance::SetVars(const float DeltaTime)
{
	CameraTransform = FTransform(Character->GetBaseAimRotation(), Character->Camera->GetComponentLocation());

	const FTransform& RootOffset = Mesh->GetSocketTransform(FName("root"), RTS_Component).Inverse()* Mesh->GetSocketTransform(FName("ik_hand_root"));
	RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);

	ADSWeight=Character->ADSWeight;

	// Accumulative rotation, 변환 이후
	constexpr float AngleClamp = 6.f;
	const FRotator& AddRotation = CameraTransform.Rotator() - LastRotation;
	FRotator AddRotationClamped = FRotator(FMath::ClampAngle(AddRotation.Pitch, -AngleClamp, AngleClamp)*1.5f,
		FMath::ClampAngle(AddRotation.Yaw, -AngleClamp, AngleClamp), 0.f);
	AddRotationClamped.Roll = AddRotationClamped.Yaw * 0.7f;

	AccumulativeRotation += AddRotationClamped;
	AccumulativeRotation = UKismetMathLibrary::RInterpTo(AccumulativeRotation, FRotator::ZeroRotator, DeltaTime, 30.f);
	AccumulativeRotationInterp = UKismetMathLibrary::RInterpTo(AccumulativeRotationInterp, AccumulativeRotation, DeltaTime, 5.f);
}

void UTrueFPSAnimInstance::CalculateWeaponSway(const float DeltaTime) //서버 변환 이후
{
	FVector LocationOffset = FVector::ZeroVector; //무기의 Location 기준점
	FRotator RotationOffset = FRotator::ZeroRotator; // 무기의 Rotation 기준점

	const FRotator& AccumulativeRotationInterpInverse = AccumulativeRotationInterp.GetInverse();
	RotationOffset += AccumulativeRotationInterpInverse;

	LocationOffset+= FVector(0.f, AccumulativeRotationInterpInverse.Yaw, AccumulativeRotationInterpInverse.Pitch) /6.f;

	LocationOffset *= IKProperties.WeightScale;
	RotationOffset.Pitch *=IKProperties.WeightScale;
	RotationOffset.Yaw *=IKProperties.WeightScale;
	RotationOffset.Roll *=IKProperties.WeightScale;
	
	OffsetTransform = FTransform(RotationOffset, LocationOffset);
}

void UTrueFPSAnimInstance::SetIKTransforms()
{
	RHandToSightsTransform=CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r"))); 
}




