// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GunAnimInstance.h"
#include "Character/TrueFPSCharacter.h"

void UGunAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	/*if (IsLeftMouseInputPressed())
	{
		IsShoot = true;
	}
	else if(IsLeftMouseInputReleased())
	{
		IsShoot = false;
		UE_LOG(LogTemp, Log, TEXT("Hello"));
	}*/
}

bool UGunAnimInstance::IsLeftMouseInputPressed()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// 스페이스바 입력을 확인합니다.
		UPlayerInput* PlayerInput = PlayerController->PlayerInput;
		if (PlayerInput)
		{
			return PlayerInput->IsPressed(FKey("LeftMouseButton"));
		}
	}

	return false;
}

bool UGunAnimInstance::IsLeftMouseInputReleased()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// 스페이스바 입력을 확인합니다.
		UPlayerInput* PlayerInput = PlayerController->PlayerInput;
		if (PlayerInput)
		{
			bool SpacebarPressed = PlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
			return !SpacebarPressed;
		}
	}

	return false;
}

UGunAnimInstance::UGunAnimInstance()
{
	IsShoot = false;
}

