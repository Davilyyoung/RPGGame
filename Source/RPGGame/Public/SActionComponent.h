
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "SActionComponent.generated.h"

class USAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, USActionComponent*, OwningComp, USAction*, Action);

//行动组件 GAS
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGGAME_API USActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ActiveTags") 
	FGameplayTagContainer ActiveGameplayTags;//技能标签Gameplaytags

	//自己简单实现的GAS 添加技能
	UFUNCTION(BlueprintCallable,Category="Actions")
	void AddAction(AActor* Instigator,TSubclassOf<USAction> ActionClass);

	/* Returns first occurrence of action matching the class provided */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	USAction* GetAction(TSubclassOf<USAction> ActionClass) const;
	
	//自己简单实现的GAS 开始释放技能
	UFUNCTION(BlueprintCallable,Category="Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	//自己简单实现的GAS 停止释放技能
	UFUNCTION(BlueprintCallable,Category="Actions")
	bool StopActionByName(AActor* Instigator,FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(USAction* ActionToRemove);
	
	USActionComponent();

protected:

	UFUNCTION(Server,Reliable)
	void ServerStartAction(AActor* Instigator,FName ActionName);
	
	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FName ActionName);
	
	//Granted abilities at game start
	UPROPERTY(EditAnywhere,Category="Actions") 
	TArray<TSubclassOf<USAction>> DefaultActions;//在游戏开始时授予的能力 面板中而不是全都要拖入蓝图

	UPROPERTY(BlueprintReadOnly, Replicated) //告诉系统记录这些 别删出
	TArray<USAction*> Actions; //存动作

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;//每当一个变量改变了它的值，网络就应该通知所有的客户端这个变化 

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStopped;

#pragma region OL
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
#pragma endregion

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
