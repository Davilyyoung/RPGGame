#include "AI/SAICharacter.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "UI/SWorldUserWidget.h"


ASAICharacter::ASAICharacter()
{
	// 听觉 视觉组件 
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));

	// 生成时候 或者 放入场景时候 自动控制AI 蓝图里面也可以调
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// Capsule胶囊的碰撞响应改成ECR_Ignore 忽略自己
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Ignore);
	// Enabled on mesh to react to incoming projectiles
	// 在网格上启用，对射入的弹丸作出反应
	GetMesh()->SetGenerateOverlapEvents(true);
	
	TargetActorKey = "TargetActor";
	TimeToHitParamName = "TimeToHit";
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);//绑定这个动态多播
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
	}
}

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	// Ignore if target already set
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);

		MulticastPawnSeen();
	}
	//DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 0.5f, true);
}

void ASAICharacter::MulticastPawnSeen_Implementation()
{
	USWorldUserWidget* NewWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
	if (NewWidget)
	{
		NewWidget->AttachedActor = this;
		// Index of 10 (or anything higher than default of 0) places this on top of any other widget.
		// May end up behind the minion health bar otherwise.
		NewWidget->AddToViewport(10);
	}
}


void ASAICharacter::OnHealthChanged(AActor* InsigatorActor, USAttributeComponent* OwningComp, float NewHealth,
                                    float Delta)
{
	if (Delta < 0.0f)
	{
		if (InsigatorActor != this)//如果没有发现我们 打AI AI可以回头打玩家？
		{
			SetTargetActor(InsigatorActor);
		}
		
		//受伤显示血条
		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(),HealthBarWidgetClass);	
		
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;//附着到AI上 
				ActiveHealthBar->AddToViewport();
			}
		}
		//受伤闪烁
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName,GetWorld()->TimeSeconds);

		// Died 死了
		if (NewHealth <= 0.0f)
		{
			//stopbt
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				//Stop BT
				AIC->GetBrainComponent()->StopLogic("Killer");// 停止AI行动
			}
			//Ragdoll布娃娃
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");//Ragdoll布娃娃

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//  死亡Capsule胶囊无碰撞
			GetCharacterMovement()->DisableMovement();//使运动不可能

			//set lifespawn
			SetLifeSpan(10.0f);
		}
	}
}







