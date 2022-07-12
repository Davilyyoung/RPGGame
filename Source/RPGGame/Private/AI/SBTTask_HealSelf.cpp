// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealSelf.h"

#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	APawn* MyAIPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if (MyAIPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(MyAIPawn);
	if (ensure(AttributeComp))
	{
		AttributeComp->ApplyHealthChange(MyAIPawn, AttributeComp->GetHealthMax());
		
	}

	return EBTNodeResult::Succeeded;
	

	//return Super::ExecuteTask(OwnerComp, NodeMemory);
}
