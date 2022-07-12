#include "SExplosiveBarrel.h"

#include "DrawDebugHelpers.h"

ASExplosiveBarrel::ASExplosiveBarrel()
{
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);//关闭物理模拟

	ForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComp"));
	ForceComp->SetupAttachment(MeshComp);
	ForceComp->SetAutoActivate(false);//true的化里面会需要去运行其他函数 我们不需要
	ForceComp->Radius = 750.0f;//爆炸半径
	ForceComp->ImpulseStrength = 2500.0f;///脉冲应该有多大的强度
	ForceComp->bImpulseVelChange = true;//如果为真，冲力将忽略物体的质量，并且总是导致固定的速度变化
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);//设置碰撞类型
	

}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	MeshComp->OnComponentHit.AddDynamic(this,&ASExplosiveBarrel::OnActorHit);//绑定被击中之后触发的时间
	
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ForceComp->FireImpulse();//这个物体被击中会释放一个冲力

	UE_LOG(LogTemp,Log,TEXT("OtherActor:%s,at Game time:%f"),*GetNameSafe(OtherActor),GetWorld()->GetTimeSeconds());

	FString CombinedString = FString::Printf(TEXT("Hit at location %s"),*Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(),Hit.ImpactPoint,CombinedString,nullptr,FColor::Green,2.0f,true);
}


