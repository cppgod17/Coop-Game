// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UParticleSystem;


//�������� ���� � ��������� �������� ����������
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	//��� ��� ����� 1 �������� ��������� ������� ������ �� ����� ������ ���������� ������ ������� �.�. ��������� �� ��������
	//�� ��������� ���� ������� ��� ��������� ��������� � ����� ������ ����� ��������� ������� ������
	UPROPERTY()
	uint8 BurstCounter;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	USkeletalMeshComponent* MeshComp = nullptr;

	void PlayFireEffect(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType,FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BaseDamage;

	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	FTimerHandle TimerHandle_TimeBeetwenShoots;

	float LastFireTime;

	// ���-�� ���� �� ������ 
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	// �������� �� ����� ��������
	float TimeBeetwenShots;


	//������� ���� � ��������
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin=0.f))
	float BulletSpreed;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
public:
	void StartFire();

	void StopFire();
};
