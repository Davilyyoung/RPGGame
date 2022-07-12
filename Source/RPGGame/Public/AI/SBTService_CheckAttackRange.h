 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere,Category="AI")
	FBlackboardKeySelector AttachRangeKey;
	/**更新下一个刻度线的时间间隔
	 如果节点不是实例化的，这个函数应该被认为是常量（不要修改对象的状态）！ 更新下一个tick间隔。*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
