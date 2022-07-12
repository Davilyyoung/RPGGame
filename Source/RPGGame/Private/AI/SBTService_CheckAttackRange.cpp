// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	// Check distance between ai pawn and target actor
	//检查ai棋子和目标棋子之间的距离
	UBlackboardComponent*  BlackboardComp =  OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))//ensure 错误时候不会崩溃，下次成功运行，
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			AAIController* MyAIController = OwnerComp.GetAIOwner();
			if (ensure(MyAIController))
			{
				APawn* AIPawn = MyAIController->GetPawn();
				if (ensure(AIPawn))
				{
					float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(),AIPawn->GetActorLocation());
					bool BWithinRange = DistanceTo < 2000.0f;
					bool bHasLOS = false;
					if (BWithinRange)//< 2000.0f
					{
						bHasLOS = MyAIController->LineOfSightTo(TargetActor);//开始检查
					}
					                                                          
					BlackboardComp->SetValueAsBool(AttachRangeKey.SelectedKeyName,(BWithinRange && bHasLOS));
				}
				
			}
		}
		
	}
	
}
