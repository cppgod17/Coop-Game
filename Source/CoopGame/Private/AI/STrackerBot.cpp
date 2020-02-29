// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Public/NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "Public/NavigationSystem.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Public/NavigationPath.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);
	
	BUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100.f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
	
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
	DrawDebugString(GetWorld(), GetActorLocation(), "DUDUDSH!");
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, BUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), NextPathPoint, 30.f, FColor::Green, false, 0.f, 0, 1.f);
	}
	DrawDebugSphere(GetWorld(), NextPathPoint, 30.f, 12, FColor::Green, false, 4.f, 1.f);
}


