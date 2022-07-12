#include "SItemChest.h"

#include "Net/UnrealNetwork.h"

ASItemChest::ASItemChest()
{
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);
	
	TargetPitch = 110.0f;
	
	// Directly set bool instead of going through SetReplicates(true) within constructor,
	// Only use SetReplicates() outside constructor
	bReplicates = true;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;//开 关

	OnRep_LidOpened();//需自己手动进行 服务器会被调用
}

void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened()//OnRep_XXXXXX函数会被调用，服务器要想调用，需自己手动进行.
{
	//这个宝箱盖子 (打开)
	float CurrPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrPitch,0,0));
}

// 属性网络同步
// 请确保 SetReplicates 变量设置为 true   
void ASItemChest::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate to every client 这个值(bLidOpened)变化
	DOREPLIFETIME(ASItemChest,bLidOpened);//宏来声明 
}




