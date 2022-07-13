#pragma once

#include "CoreMinimal.h"
#include "SGamePlayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class RPGGAME_API ASItemChest : public AActor,public ISGamePlayInterface//继承了这个接口
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	float TargetPitch;

	//重写接口
	void Interact_Implementation(APawn* InstigatorPawn);//实现这个接口方法

	void OnActorLoaded_Implementation();

protected:
	
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;//每当一个变量改变了它的值，网络就应该通知所有的客户端这个变化 

	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened" ,BlueprintReadOnly,SaveGame)//可复制  服务器和客户端进行同步
	bool bLidOpened;

	UFUNCTION()
	void OnRep_LidOpened();//只在客户端上触发
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

public:
	//构造
	ASItemChest();
	

};
