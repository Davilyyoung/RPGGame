// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "SGameModeBase.generated.h"

//DataTable Row for spawning monsters in game mode
USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;  // 使用资产管理器 通过资产ID

	//TSubclassOf<AActor> MonsterClass;

	/* Relative chance to pick this monster *//* 挑选这个怪物的相对机会 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by gamemode to spawn this unit. */	/* 游戏模式所需的点数，以催生这个单位。*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits awarded to killer of this unit.  */	/* 授予该单位杀手的点数。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;

};


UCLASS()
class RPGGAME_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:

	ASGameModeBase();
	

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(Exec)
	void KillAllAI();

	
	virtual void OnActorKilled(AActor* VictimActor,AActor* Killer);//VictimActor受害人  Killer杀手 

	
protected:

	/* All available monsters */
	UPROPERTY(EditDefaultsOnly,Category="AI")
	UDataTable* MonsterTable;
	
	// UPROPERTY(EditDefaultsOnly,Category="AI")
	// TSubclassOf<AActor> MinionClass;
	
	UPROPERTY(EditDefaultsOnly,Category= "AI")
	UEnvQuery* SpawnBotQuery;
	
	UPROPERTY(EditDefaultsOnly,Category= "AI")
	UCurveFloat* DifficultyCurve;//难度曲线


	UPROPERTY(EditDefaultsOnly,Category="AI")
	float SpawnTimerInterval;//间隔时间

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);
	
	FMonsterInfoRow* SelectedMonsterRow;

	/* Time to wait between failed attempts to spawn/buy monster to give some time to build up credits. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float CooldownTimeBetweenFailures;
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);//重生 不能是ACharerctor因为已经死了

	UPROPERTY(EditDefaultsOnly,Category="Player")
	float RespawnDelay;


	// Read/write access as we could change this as our difficulty increases via Blueprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditsPerKill;//死亡加多少份

	/* Curve to grant credits to spend on spawning monsters */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* SpawnCreditCurve;

	// Points available to spend on spawning monsters
	float AvailableSpawnCredit;

	/* GameTime cooldown to give spawner some time to build up credits */
	float CooldownBotSpawnUntil;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	UEnvQuery* PowerupSpawnQuery;

	/* All power-up classes used to spawn with EQS at match start */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<AActor>> PowerupClasses;

	/* Distance required between power-up spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float RequiredPowerupDistance;

	/* Amount of powerups to spawn during match start */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 DesiredPowerupCount;

	UFUNCTION()
	void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	FTimerHandle TimerHandle_SpawnBots;

	
};
