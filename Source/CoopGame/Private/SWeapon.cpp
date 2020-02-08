// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	//Пускаем трейс от глаз игрока до локаций прицела
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// определения локации точки конца трейса
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FHitResult Hitresult;
		if (GetWorld()->LineTraceSingleByChannel(Hitresult, EyeLocation, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			// если попали, то обрабатываем урон, функция возвращает булевое значение
			AActor* HitActor = Hitresult.GetActor();
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hitresult, MyOwner->GetInstigatorController(), this, DamageType);
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hitresult.ImpactPoint, Hitresult.ImpactNormal.Rotation());
			}

		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 5.f, 0, 1.f);

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

