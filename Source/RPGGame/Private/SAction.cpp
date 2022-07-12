#include "SAction.h"
#include "SActionComponent.h"
#include "Net/UnrealNetwork.h"
#include "RPGGame/RPGGame.h"

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if (IsRunning())
	{
		return false;//不应该允许启动一个已经启动的动作
	}
	
	USActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockTags))
	{
		return false;//不能做这个动作
	}
	
	return true;//可以做这个动作
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Started: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	USActionComponent* Comp = GetOwningComponent();	
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
	

	if (GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);//打中显示血条上收伤UI
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

    //ensureAlways(bIsRunning);

	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);//打中显示血条上收伤UI
}



// 确保我们得到的世界 真正正确的在工作在我们的Uobject(对象里)
// 虽然GetWorld()已经在我们的Uobject里
// UObject里不GetWorld
// 重写GetWorld是为了在蓝图中访问
// 如果不重写GetWorld 那么蓝图中无法访问SpawnActor SweepSingleByObjectType 或者其他静态函数 。在蓝图中无法显示
UWorld* USAction::GetWorld() const
{
	// Outer is set when Creating action via NewObject<T>  当通过NewObject<T>创建动作时，外层被设置。
	AActor* Actor = Cast<AActor>(GetOuter());//实例化它的时设置的东西 创建的新对象
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	//AActor* Actor = Cast<AActor>(GetOuter());
	//return Actor->GetComponentByClass(USActionComponent::StaticClass());

	return ActionComp;
}



bool USAction::IsRunning() const
{
	return RepData.bIsRunning;
}

void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	
	DOREPLIFETIME(USAction, RepData);//复制的数据
	DOREPLIFETIME(USAction, TimeStarted);
	DOREPLIFETIME(USAction, ActionComp);

}

