// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"


enum class EWaveState : uint8;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()



protected:
    FTimerHandle TimerHandle_BotSpawner;

    FTimerHandle TimerHandle_NextWaveStart;
    //кол-во ботов для спавна в текущей волне
    int32 NrOfBotsToSpawn;

    int32 WaveCount;

    UPROPERTY(EditDefaultsOnly,Category = "GameMode")
    float TimeBeetweenWaves;
protected:
    UFUNCTION(BlueprintImplementableEvent,Category = "GameMode")
    void SpawnNewBot();

    void SpawnBotTimerElapsed();

    //Начал спавна волны ботов
    void StartWave();

    //Конец спавна волны ботов
    void EndWave();

    //Установка таймера для призыва следующей волны ботов
    void PrepareForNextWave();

    void CheckWaveState();

    void CheckAnyPlayerAlive();

    void GameOver();

    void SetWaveState(EWaveState NewState);
public:
    ASGameMode();

    virtual void StartPlay() override;

    virtual void Tick(float DeltaSeconds) override;
};
