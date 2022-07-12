// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

//TAutoConsoleVariable控制台变量 启用伤害成算																			//属性组合的全球伤害修改器
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.CVarDamageMultiplier"),1.0f,TEXT("Globla Damage Modifier for Attribute Compnent."),ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{

	HealthMax = 100;
	Health = HealthMax;

	Rage = 0;
	RageMax = 100;

	//Component组件的构造函数中才能用
	//SetIsReplicatedByDefault当只是在组件的构造函数中设置它时候用
	SetIsReplicatedByDefault(true);//这应该只在组件构建过程中被调用。这个方法的存在只是为了让代码能够直接修改bReplicates以保持现有的
	
}

//杀死所有
bool USAttributeComponent::Kill(AActor* InstigatorActor)
{ 
	return ApplyHealthChange(InstigatorActor,-GetHealthMax());
}


bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool USAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}


float USAttributeComponent::GetHealth() const
{
	return Health;
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}


bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor,float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)//无敌　控制台命令God
	{
		return false;
	}
	
	if (Delta < 0.0f)//作弊该伤害
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
		
	}
	
	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = NewHealth - OldHealth;

	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;	
		if (ActualDelta != 0.0f)//没死才发送给客户端广播 不浪费
	    {
			//OnHealthChanged.Broadcast(InstigatorActor,this,Health,AcualDelta);//调用多播
			MulticastHealthChanged(InstigatorActor,Health,ActualDelta);//服务器调用 所有客户端会收到
		}

		//Died死亡
		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			//GetAuthGameMode返回当前的游戏模式实例，它在服务器上的游戏过程中始终有效。这将只在服务器上返回一个有效的指针。在客户端将总是返回空
			ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();//得到我们自己的GameMode
			if (GM)
			{
				GM->OnActorKilled(GetOwner(),InstigatorActor);
			}
			
		}
	}
	
	return ActualDelta != 0; //返回一个值，了解它是否真的产生了变化或者没有
}

float USAttributeComponent::GetRage() const
{
	return Rage;
}



bool USAttributeComponent::ApplyRage(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);

	float ActualDelta = Rage - OldRage;
	if (ActualDelta != 0.0f)
	{
			OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);
	}

	return ActualDelta != 0;
}





USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}



bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	return false;
}



void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor,this,NewHealth,Delta);
}


void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}



void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(USAttributeComponent, Health);
		DOREPLIFETIME(USAttributeComponent, HealthMax);

		DOREPLIFETIME(USAttributeComponent, Rage);
		DOREPLIFETIME(USAttributeComponent, RageMax);
}