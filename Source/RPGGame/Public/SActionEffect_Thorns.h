// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SAttributeComponent.h"
#include "SActionEffect_Thorns.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USActionEffect_Thorns : public USActionEffect
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Thorns")
	float ReflectFraction;//反射率

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

public:

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

	USActionEffect_Thorns();
};
