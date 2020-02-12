// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);


// Sets default values
ASWeapon::ASWeapon()
{
	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TraceTargetName = "BeamEnd";
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

		//Цель куда идет партикл
		FVector TracerEndPoint = TraceEnd;

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
			TracerEndPoint = Hitresult.ImpactPoint;
		}

		if (DebugWeaponDrawing>0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 5.f, 0, 1.f);
		}
		PlayFireEffect(TracerEndPoint);
	}
}

void ASWeapon::PlayFireEffect(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TraceTargetName, TraceEnd);
		}
	}
}

