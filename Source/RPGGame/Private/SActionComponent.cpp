

#include "SActionComponent.h"
#include "SAction.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "RPGGame/RPGGame.h"

DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_HALL);



USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);//默认复制 
}

void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server Only
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}


void USActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Stop all
	TArray<USAction*> ActionsCopy = Actions;//做一个副本
	for (USAction* Action : ActionsCopy)//比哪里副本
	{
		if (Action && Action->IsRunning())//这个副本还在运行？
		{
			Action->StopAction(GetOwner());//让他停止
		}
	}

	Super::EndPlay(EndPlayReason);
}

void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	// Draw All Actions
		for (USAction* Action : Actions)
		{
			FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
			FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s :"),
				*GetNameSafe(GetOwner()),*GetNameSafe(Action));
	
			LogOnScreen(this, ActionMsg, TextColor, 0.0f);
		}
}

void USActionComponent::AddAction(AActor* Instigator,TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))//为空返回
	{
		return;
	}
	
	// Skip for clients
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}
	
	USAction* NewAction = NewObject<USAction>(GetOwner(),ActionClass);
	if (ensure(NewAction))
	{
		NewAction->Initialize(this);//每当我们要把这些新的对象实例化时
		//保证有 然后加入Action数组中
		Actions.Add(NewAction);
	
		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}

	Actions.Remove(ActionToRemove);

}
USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{

	for (USAction* Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}


bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);//范围循环统计器
	
	for (USAction* Action : Actions) //Actions 不能超Actions数组里面存的 基于这个范围来遍历
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))//如果不能启动
			{
				//Debug验证技能阻挡有效性
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"),*ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1,2.0f,FColor::Red,FailedMsg);

				continue;//整个从循环中返回
			}

			//GetOwner这将是这个所依附得行为者 然后就可以调用拥有权 !如果我们时客户端 我们要调用一个服务器RPC  不返回 继续运行
			if (!GetOwner()->HasAuthority())//只调用服务器启动动作
			{
				//向Server发送一个请求
				ServerStartAction(Instigator,ActionName);//所有方式再其他地方和世界的另一端 请为我启动这个行动
			}
			
			// Bookmark for Unreal Insights// 为虚幻的洞察力添加书签　项目打包后 源码里面有启动器 启动打包游戏 才能用
			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action));
			
			Action->StartAction(Instigator);//释放技能
			return true;
		}
	}
	return false;
}


bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions) //Actions 不能超Actions数组里面存的 基于这个范围来遍历　//使用基于范围的for循环
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())//是在释放这个技能
			{
				// Is Client?
				if (!GetOwner()->HasAuthority())
				{
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

//ServerStartAction_Implementation被调用的话 就成功进入服务器
void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}


void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}



//复制之我对象
bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	for (USAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;//告诉UE这里面有一个组件部分变化 请复制数据
}


void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);	


}