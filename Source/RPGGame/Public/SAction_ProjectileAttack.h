// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()

protected:

	/* Sphere radius of the sweep to find desired target under crosshair. Adjusts final projectile direction */
	UPROPERTY(EditAnywhere, Category="Targeting")
	float SweepRadius;

	/* Fallback distance when sweep finds no collision under crosshair. Adjusts final projectile direction */
	UPROPERTY(EditAnywhere, Category="Targeting")
	float SweepDistanceFallback;

	UPROPERTY(EditAnywhere,Category="Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere,Category="Attack")
	UAnimMontage* AttackMontage;

	//Particle system played during attack animation 在攻击动画中播放的粒子系统
	UPROPERTY(EditAnywhere,Category="Attack")
	UParticleSystem* CastingEffect;

	/* Sound Effect to play (Can be Wave or Cue) */
	UPROPERTY(EditAnywhere, Category = "Attack")
	USoundBase* CastingSound;

	UPROPERTY(EditAnywhere,Category="Attack")
	float AttackAnimDelay;

	
	UPROPERTY(EditAnywhere,Category="Effects")
	FName HandSocketName;

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);//计时器功能

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	USAction_ProjectileAttack();
};
