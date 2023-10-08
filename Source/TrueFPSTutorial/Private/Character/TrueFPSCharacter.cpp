

#include "Character/TrueFPSCharacter.h"
#include "..\..\Public\Character\TrueFPSCharacter.h"
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>
#include "Weapons/Weapon.h"
#include "Camera/CAmeraComponent.h"

ATrueFPSCharacter::ATrueFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // �ε巴�� ��ȣ�ۿ��� �ϱ� ���� Tick�� �����

	GetMesh()->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	GetMesh()->bVisibleInReflectionCaptures=true;
	GetMesh()->bCastHiddenShadow=true;

	ClientMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClientMesh"));
	ClientMesh->SetCastShadow(false);
	ClientMesh->bCastHiddenShadow=false;
	ClientMesh->bVisibleInReflectionCaptures = false;
	ClientMesh->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	ClientMesh->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true; 
	Camera->SetupAttachment(GetMesh(), FName("head")); 
}

void ATrueFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Setup ADS timeline
	if(AimingCurve)
	{
		FOnTimelineFloat TimelineFloat;
		TimelineFloat.BindDynamic(this, &ATrueFPSCharacter::TimeLineProgress);

		AimingTimeline.AddInterpFloat(AimingCurve, TimelineFloat);	
	}

	//Client Mesh logic
	if(IsLocallyControlled())
	{
		ClientMesh->HideBoneByName(FName("neck_01"),EPhysBodyOp::PBO_None);
		GetMesh()->SetVisibility(false);
	}
	else
	{
		ClientMesh->DestroyComponent();
	}
	
	// Spawning weapons
	if (HasAuthority())
	{
		for (const TSubclassOf<AWeapon>& WeaponClass : DefaultWeapons)
		{
			if (!WeaponClass) continue;
			FActorSpawnParameters Params;
			Params.Owner = this;
			AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Params);
			const int32 Index = Weapons.Add(SpawnedWeapon);
			if (Index == CurrentIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
}

void ATrueFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, CurrentWeapon, COND_None);
	DOREPLIFETIME_CONDITION(ATrueFPSCharacter, ADSWeight, COND_None); //������ ����� ����
}

void ATrueFPSCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(ATrueFPSCharacter, ADSWeight, ADSWeight >= 1.f || ADSWeight <=0.f); //ADSWeight�� ���� ���� �����̸� Ȱ��ȭ
}


void ATrueFPSCharacter::OnRep_CurrentWeapon(const AWeapon* OldWeapon)
{
	if (CurrentWeapon)
	{
		if (!CurrentWeapon->CurrentOwner)
		{
			const FTransform PlacementTransform = CurrentWeapon->PlacementTransform * GetMesh()->GetSocketTransform(FName("hand_r"));
			CurrentWeapon->SetActorTransform(PlacementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("hand_r"));

			CurrentWeapon->CurrentOwner = this;
		}

		CurrentWeapon->Mesh->SetVisibility(true);
	}

	if (OldWeapon)
	{
		OldWeapon->Mesh->SetVisibility(false);
	}

	CurrentWeaponChangedDelegate.Broadcast(CurrentWeapon, OldWeapon);
}

void ATrueFPSCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimingTimeline.TickTimeline(DeltaTime); // Aiming�ϴ� �ð�
}

void ATrueFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Pressed, this, &ATrueFPSCharacter::StartAiming); //Aiming�� ���� �Է�
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Released, this, &ATrueFPSCharacter::ReverseAiming); // Aiming ��Ҹ� ���� �Է�
	
	PlayerInputComponent->BindAction(FName("NextWeapon"), EInputEvent::IE_Pressed, this, &ATrueFPSCharacter::NextWeapon);
	PlayerInputComponent->BindAction(FName("LastWeapon"), EInputEvent::IE_Pressed, this, &ATrueFPSCharacter::LastWeapon);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ATrueFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ATrueFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ATrueFPSCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("LookRight"), this, &ATrueFPSCharacter::LookRight);
}

void ATrueFPSCharacter::EquipWeapon(const int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon==Weapons[Index]) return;

	if (IsLocallyControlled() || HasAuthority())
	{
		CurrentIndex = Index;

		const AWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);
	}
	
	if (!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
	}
}

void ATrueFPSCharacter::Server_SetCurrentWeapon_Implementation(AWeapon* NewWeapon)
{
	const AWeapon* OldWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}

void ATrueFPSCharacter::StartAiming()
{
	if(IsLocallyControlled() || HasAuthority())
	{
		Multi_Aim_Implementation(true); // Aiming�� �ϵ��� ���
	}

	if(!HasAuthority())
	{
		Server_Aim(true); //�������� Aiming�� �϶�� ����
	}
}

void ATrueFPSCharacter::ReverseAiming()
{
	if(IsLocallyControlled() || HasAuthority())
	{
		Multi_Aim_Implementation(false); // Aiming�� ���� �ʵ��� ���
	}

	if(!HasAuthority())
	{
		Server_Aim(false); //�������� Aiming�� ���� �ʵ��� ����
	}
}

void ATrueFPSCharacter::Multi_Aim_Implementation(const bool bForward)
{
	if(bForward)
	{
		AimingTimeline.Play();
	}
	else
	{
		AimingTimeline.Reverse();
	}
}

void ATrueFPSCharacter::TimeLineProgress(const float Value)
{
	ADSWeight=Value;
}


void ATrueFPSCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);
}

void ATrueFPSCharacter::LastWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num()-1;
	EquipWeapon(Index);
}

void ATrueFPSCharacter::MoveForward(const float Value)
{
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void ATrueFPSCharacter::MoveRight(const float Value)
{
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void ATrueFPSCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

void ATrueFPSCharacter::LookRight(const float Value)
{
	AddControllerYawInput(Value);
}

