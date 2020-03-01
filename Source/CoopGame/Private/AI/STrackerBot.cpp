// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Public/NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "Public/NavigationSystem.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Public/NavigationPath.h"
#include "SHealthComponent.h"
#include "Public/SCharacter.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);

	HealthComp = CreateDefaultSubobject<USHealthComponent>("HealthComp");
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100.f;

	ExplosionRadius = 250.f;
	ExplosionDamage = 40.f;

	SelfDamageInterval = 0.5f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0)
	{
		SelfDestruct();
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName())
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

void ASTrackerBot::SelfDestruct()
{
	if (bExploded) { return; }

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.f, 0, 1.f);

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	Destroy();
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20.f, GetInstigatorController(), this, nullptr);
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

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), NextPathPoint, 30.f, FColor::Green, false, 0.f, 0, 1.f);
	}
	DrawDebugSphere(GetWorld(), NextPathPoint, 30.f, 12, FColor::Green, false, 4.f, 1.f);
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bStartedSelfDestructing)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn)
		{
			GetWorldTimerManager().SetTimer(TimerHandleSelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.f);

			bStartedSelfDestructing = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}

}

