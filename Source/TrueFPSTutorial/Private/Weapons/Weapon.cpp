// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"
#include <Kismet/GameplayStatics.h>
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 틱으로 bool 함수를 통해서 연사를 조절한다.

	SetReplicates(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	RemainingTime = 0;
	IsWeaponDelay = false;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay(); // BeginPlay를 맨 처음으로 두어야 오류가 나지 않는다.

	if(!CurrentOwner)
		Mesh->SetVisibility(false);
}

void AWeapon::Tick(const float DeltaTime)
{
	if (CurrentBullet > 0)
	{
		bShoot = true;
	}
	else
	{
		bShoot = false;
	}
}

void AWeapon::Shooting()
{
	if (bShoot && !IsWeaponDelay)
	{
		if (GunKind != 3)
		{
			CurrentBullet--;
			UE_LOG(LogTemp, Log, TEXT("Current Bullet : %d"), CurrentBullet);

			if (FireAnimation)
			{
				Mesh->PlayAnimation(FireAnimation, false);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AWeapon::Shooting, ShootDelay, true);
			}
		}

		TraceTarget();
	}
}

void AWeapon::StopShooting()
{
	if (!IsWeaponDelay && GunKind != 3)
	{
		IsWeaponDelay = true;
		if (FireAnimation)
		{
			GetWorld()->GetTimerManager().PauseTimer(TimerHandle);
			RemainingTime = GetWorldTimerManager().GetTimerRemaining(TimerHandle);

			FTimerHandle DelayTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &AWeapon::Delaying, RemainingTime, false);
		}
	}
}

void AWeapon::Delaying()
{
	UE_LOG(LogTemp, Log, TEXT("Weapon : Delay end"));
	IsWeaponDelay = false;
}

void AWeapon::Reloading()
{
	Mesh->PlayAnimation(ReloadGunAnim, false);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeapon::ReloadDone, ReloadTime, false);
}

void AWeapon::ReloadDone()
{
	CurrentBullet = MaxBullet;
	IsWeaponDelay = false;
}

void AWeapon::TraceTarget()
{
	FHitResult Hit;
	FVector ShotDirection;
	bool IsTarget = GunTrace(Hit, ShotDirection);

	if (IsTarget)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
	}
}

AController* AWeapon::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
		return nullptr;
	return  OwnerPawn->GetController();
}

bool AWeapon::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr)
		return false;

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();

	FVector End = Location + Rotation.Vector() * MaxRange;
	DrawDebugSphere(GetWorld(), End, 50, 32, FColor::Red, false, 5.0f); // 총 발사 거리
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	return  GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}



