// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "TimerManager.h"
#include "Component/SHealthComponent.h"

ASGameMode::ASGameMode()
{
	TimeBeetweenWaves = 2.f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
}



void ASGameMode::StartWave()
{
	WaveCount++;
	
	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.f, true, 0.f);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBeetweenWaves, false);
}


void ASGameMode::CheckWaveState()
{

	bool bIsPreapearingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreapearingForWave)
	{
		return;
	}
	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn==nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}
		USHealthComponent* HealthComponent = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}
	if (!bIsAnyBotAlive)
	{
		PrepareForNextWave();
	}
	
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckWaveState();
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}
