// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGamePlayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USGamePlayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//外部要一直调用
public:
	//static函数可以直接类名＋::调用此函数
	UFUNCTION(BlueprintCallable,Category="GamePlay")//一般伤害
	static bool ApplyDamage(AActor* DamageCauser,AActor* TargetActor,float DamageAmount);
	
	//static函数可以直接类名＋::调用此函数  
	UFUNCTION(BlueprintCallable,Category="GamePlay")//冲击伤害
	static bool ApplyDirectionalDamage(AActor* DamageCauser,AActor* TargetActor,float DamageAmount, const FHitResult& HitResult);//应用方向性损伤
	
};
