// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionComponent.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	/* Is Pawn Stunned based on GameplayTag data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	USActionComponent* ActionComp;

	//再本地初始化的动画 像像BeginPlay
	virtual void NativeInitializeAnimation() override;

	//再本地初始化的动画 像蓝图中的Update tick
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	
};
