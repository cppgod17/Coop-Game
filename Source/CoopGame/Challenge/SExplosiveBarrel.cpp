// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Component/SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
    HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
    HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetSimulatePhysics(true);
    // ���������� ���� ����� ������ ��������� ��� ������ �� ������
    MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

    RootComponent = MeshComp;

    RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
    RadialForceComp->SetupAttachment(MeshComp);
    RadialForceComp->Radius = 250.f;
    RadialForceComp->bImpulseVelChange = true;
    RadialForceComp->bAutoActivate = false;
    RadialForceComp->bIgnoreOwningActor = true;
    
    ExplosionImpulse = 400.f;

    SetReplicates(true);
    SetReplicateMovement(true);
}
// ���������� �� ������� ��� ��� ������� � HEALTHCOMPONENT ����� ������
void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningfHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExspoded) { return; }
	if (Health <= 0)
	{
		bExspoded = true;
        OnRepExploded();
		//���� ������� �� �� ������ (0,0,1)
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.f, GetActorLocation(), 300.f, DamageTypee, TArray<AActor*>());
		RadialForceComp->FireImpulse();

	}
 
}

void ASExplosiveBarrel::OnRepExploded()
{
	//�������� �������
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	//������ ��������
	MeshComp->SetMaterial(0, ExplodedMaterial);
}


void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bExspoded);
}
