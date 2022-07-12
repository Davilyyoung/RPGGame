// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpread = 2.0f;
}

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{       
	AAIController* MyAIController = OwnerComp.GetAIOwner();
	if (ensure(MyAIController))
	{
		//拿到AIPawn
		ACharacter* MyAIPawn = Cast<ACharacter>(MyAIController->GetPawn());
		if (MyAIPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		//得到AI角色挂架上的枪口位置
		FVector MuzzleLocation = MyAIPawn->GetMesh()->GetSocketLocation("Muzzle_01");
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if(!USAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
			
		}
		
		//AI和玩家的距离 AI枪口方向对准玩家  (攻击的方向)
		FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();

		MuzzleRotation.Pitch += FMath::RandRange(0.0f,MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread,MaxBulletSpread);
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = MyAIPawn;//技能碰撞体积过大 别打到自己

		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass,MuzzleLocation,MuzzleRotation,Params);

		return  NewProj? EBTNodeResult::Succeeded : EBTNodeResult::Failed;//是否创建NewProj成功返回Succeeded 不成功返回Failed
	}
	
	return EBTNodeResult::Failed;

	//return Super::ExecuteTask(OwnerComp, NodeMemory);
}

