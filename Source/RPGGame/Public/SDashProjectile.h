#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "GameFramework/Actor.h"
#include "SDashProjectile.generated.h"
//闪烁技能 继承自ASProjectileBase
UCLASS()
class RPGGAME_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
public:	
	ASDashProjectile();

protected:
	UPROPERTY(EditDefaultsOnly,Category="Teleport")//传送
	float TeleportDelay;//远距传输延时

	UPROPERTY(EditDefaultsOnly,Category="Teleport")//传送
	float DetonateDelay;//引爆延迟

	FTimerHandle TimerHandle_DelayedDetonate;//如果我们已经击中了什么东西，就用TimerHandle_DelayedDetonate来取消定时器

	virtual void Explode_Implementation() override;//使BlueprintNativeEevent的基类，我们必须重写_Implementation,而不是Explode
	
	void TeleportInstigator();//传送者

	virtual void BeginPlay() override;
	
};
