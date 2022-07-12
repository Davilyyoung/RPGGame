// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionComponent.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"


/** 
 * 
 */


USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};


UCLASS(Blueprintable)//UObject该C++类允许创建蓝图类
class RPGGAME_API USAction : public UObject
{
	GENERATED_BODY()

public:

	//只会运行再服务器上
	void Initialize(USActionComponent* NewActionComp);//初始化函数只要传过去我们想要的组件 我们想把这个组件作为我们的0组件
	
	/* Start immediately when added to an action component *//* 当添加到一个动作组件时立即启动 */
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;
	
	UFUNCTION(BlueprintCallable,Category = "Action")
	bool IsRunning() const;
	
	UFUNCTION(BlueprintNativeEvent,Category = "Action")
	bool CanStart(AActor* Instigator);

	// FName 标识ID使用
	UPROPERTY(EditDefaultsOnly,Category = "Action")
	FName ActionName;// 通过动作组件 停止和启动这些动作

	// FName 标识ID使用 
	UFUNCTION(BlueprintNativeEvent,Category = "Action")
	void StartAction(AActor* Instigator);// Instigator谁负责启动 //BlueprintNativeEvent蓝图允许覆盖

	// FName 标识ID使用
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category = "Action")
	void StopAction(AActor* Instigator);// Instigator谁负责停止 //BlueprintNativeEvent蓝图允许覆盖
	
	UWorld* GetWorld() const override;//覆盖掉GetWorld()  重写它
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon; //TSoftObjectPtr软对象引用
	
	//Tags added to owning actor when activated , Removed when action stops 当激活时，标签被添加到拥有的行为者身上，当行动停止时被移除。
	UPROPERTY(EditDefaultsOnly,Category = "ActionTags")//GameplayTags 要在ProjectSetting里设定
	FGameplayTagContainer GrantsTags;	// Tags容器标签

	//Action can only start if OwningActor has none of these Tags applied 只有在OwningActor没有应用这些标签的情况下，行动才能开始。
	UPROPERTY(EditDefaultsOnly,Category = "ActionTags")//GrantsTags这个标签阻挡BlockTags这个标签
	FGameplayTagContainer BlockTags;	// Tags容器   是否被允许运行 将再次检查相同的数组 检查是否包含任何标签 


	
	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;



#pragma region OL

	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;

	
	UFUNCTION()
	void OnRep_RepData();
	
	UPROPERTY(Replicated)
	USActionComponent* ActionComp;
	
	UPROPERTY(Replicated)
	float TimeStarted;

	
	/** IsSupportedForNetworking表示一个对象可以通过网络被引用 */
	bool IsSupportedForNetworking() const override
	{
		return true;
	}

#pragma endregion
};
