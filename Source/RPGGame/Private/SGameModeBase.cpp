// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGameSubsystem.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/GameplayStatics.h"
#include "RPGGame/RPGGame.h"

//TAutoConsoleVariable控制台变量 启用 是否关闭生成敌人
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"),true,TEXT("Eneble spawning of bots via timer."),ECVF_Cheat);



ASGameModeBase::ASGameModeBase()
{
	//不用去拖入gamemode
	//设置ASCharacter为我们自己的Character
	//DefaultPawnClass = ASCharacter::StaticClass();
	//设置ASPlayerState为我们自己的PlayState
	PlayerStateClass = ASPlayerState::StaticClass();
	//设置ASPlayerController为我们自己的PlayerController
	PlayerControllerClass = ASPlayerController::StaticClass();

	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 20;
	CooldownTimeBetweenFailures = 8.0f;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;
	
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into new SaveGameSubsystem)
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();

	// Optional slot name (Falls back to slot specified in SaveGameSettings class/INI otherwise)
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}


void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// continuous timer to spawn in more bots
	// actual amount of bots and whether its allowed to spawn determined by spawn logic later in the chain
	// 连续的计时器，以催生更多的机器人
	// 实际的机器人数量以及是否允许其产卵由链中后期的产卵逻辑决定。
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	// Make sure we have assigned at least one power-up class
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Run EQS to find potential power-up spawn locations
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
	
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we're ready to override spawn location
	// Alternatively we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	SG->OverrideSpawnTransform(NewPlayer);
}



void ASGameModeBase::KillAllAI()
{
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)//获取所有场景中的AI
	{
		ASAICharacter* Bot = *It;
		                                       //Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributesComponent::StaticClass()));
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);//@fieme: pass in player? for kill credits?
		}
	}
	
}

//定时器绑定的逻辑
void ASGameModeBase::SpawnBotTimerElapsed()
{
	//Create a float, int or string console variable
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		return; //false 直接返回 不生成敌人
	}
	
	// Give points to spend
	if (SpawnCreditCurve)
	{
		AvailableSpawnCredit += SpawnCreditCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	if (CooldownBotSpawnUntil > GetWorld()->TimeSeconds)
	{
		// Still cooling down
		return;
	}
	LogOnScreen(this, FString::Printf(TEXT("Available SpawnCredits: %f"), AvailableSpawnCredit));

	// Count alive bots before spawning
	int32 NrOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	const float MaxBotCount = 40.0f;
	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}
	
	
	if (MonsterTable)
	{
		// Reset before selecting new row
		SelectedMonsterRow = nullptr;

		TArray<FMonsterInfoRow*> Rows;
		MonsterTable->GetAllRows("", Rows);

		// Get total weight
		float TotalWeight = 0;
		for (FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;
		}

		// Random number within total random
		int32 RandomWeight = FMath::RandRange(0.0f, TotalWeight);

		//Reset
		TotalWeight = 0;

		// Get monster based on random weight
		for (FMonsterInfoRow* Entry : Rows)
		{
			TotalWeight += Entry->Weight;

			if (RandomWeight <= TotalWeight)
			{
				SelectedMonsterRow = Entry;
				break;
			}
		}

		if (SelectedMonsterRow && SelectedMonsterRow->SpawnCost >= AvailableSpawnCredit)
		{
			// Too expensive to spawn, try again soon
			CooldownBotSpawnUntil = GetWorld()->TimeSeconds + CooldownTimeBetweenFailures;

			LogOnScreen(this, FString::Printf(TEXT("Cooling down until: %f"), CooldownBotSpawnUntil), FColor::Red);
			return;
		}
	}
	
	// Run EQS to find valid spawn location
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
	
}

//事件逻辑
void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0) && MonsterTable)
	{	
		if (UAssetManager* Manager = UAssetManager::GetIfValid())//UAssetManager　异步资源加载
		{
			LogOnScreen(this, "loading monster", FColor::Green);
			
			// Apply spawn cost//应用spawn成本
			AvailableSpawnCredit -= SelectedMonsterRow->SpawnCost;

			FPrimaryAssetId MonsterId = SelectedMonsterRow->MonsterId;//id TypeName

			TArray<FName> Bundles;//空的捆绑包

			//可以流转的委托	
			FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, MonsterId, Locations[0]);
			
			Manager->LoadPrimaryAsset(MonsterId, Bundles, Delegate); //启动异步加载
		}	
	}
}


void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	//LogOnScreen(this, "Finished loading.", FColor::Green);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));//最终返回关务的所有data
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grant special actions, buffs etc.
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
}



void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Keep used locations to easily check distance between points
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	// Break out if we reached the desired count or if we have no more potential positions remaining
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// Pick a random location from remaining points.
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];
		// Remove to avoid picking again
		Locations.RemoveAt(RandomLocationIndex);

		// Check minimum distance requirement
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped locations due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}

		// Failed the distance test
		if (!bValidLocation)
		{
			continue;
		}

		
		// Pick a random powerup-class
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// Keep for distance checks
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}


//重生过期玩家
void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	//重新创建玩家
	if (ensure(Controller))
	{
		Controller->UnPossess();//Controller接触对死亡前那个死亡的玩家占有，让那个玩家变成一个单独的傀儡;
		RestartPlayer(Controller);//重新生成一个玩家
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	// Handle Player death
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		// Disabled auto-respawn
// 		FTimerHandle TimerHandle_RespawnDelay;
// 		FTimerDelegate Delegate;
// 		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());
// 
// 		float RespawnDelay = 2.0f;
// 		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);

		// Store time if it was better than previous record
		ASPlayerState* PS = Player->GetPlayerState<ASPlayerState>();
		if (PS)
		{
			PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		}

		USSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USSaveGameSubsystem>();
		// Immediately auto save on death
		SG->WriteSaveGame();
	}

	// Give Credits for kill
	APawn* KillerPawn = Cast<APawn>(Killer);
	// Don't credit kills of self
	if (KillerPawn && KillerPawn != VictimActor)
	{
		// Only Players will have a 'PlayerState' instance, bots have nullptr
		ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>();
		if (PS) 
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}

