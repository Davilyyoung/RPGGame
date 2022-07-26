// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	
	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Duration;//持续时间

	/* Time between 'ticks' to apply effect *//* 应用效果的 "滴答 "间隔时间 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Period;//间隔多少时间造成一次伤害;

	FTimerHandle PeriodHandle;
	FTimerHandle DurationHandle;

	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
    void ExecutePeriodicEffect(AActor* Instigator);//数值改变之后调用的函数

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	float GetTimeRemaining() const;

	USActionEffect();
};
