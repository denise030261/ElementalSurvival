// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"

void AEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	MoveToActor(PlayerPawn, 200);
	SetFocus(PlayerPawn);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}
