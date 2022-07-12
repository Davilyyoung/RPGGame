// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere,Category="AI Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere,Category="AI Attack")
	float MaxBulletSpread;//最大弹丸范围

public:
	USBTTask_RangedAttack();

	
};
