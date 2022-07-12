// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckHealth.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USBTService_CheckHealth : public UBTService
{
	GENERATED_BODY()

public:

	USBTService_CheckHealth();

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector LowHealthKey;

	/* Threshold that is considered 'low health' (eg. 0.3 is 30% of maximum hitpoints) */
	/* 被认为是 "低健康 "的阈值（例如，0.3是最大命中率的30%） */
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin="0.0", ClampMax="1.0"))
	float LowHealthFraction;
	/**更新下一个刻度线的时间间隔
    如果节点不是实例化的，这个函数应该被认为是常量（不要修改对象的状态）！ 更新下一个tick间隔。*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
