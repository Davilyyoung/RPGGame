#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "SProjectileBase.generated.h"

UCLASS(Abstract)//直接作成することができない抽象的な基底クラス。 サブクラスは作成する必要があり、サブクラスはインスタンスを作成することができる。
class RPGGAME_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:
	
	ASProjectileBase();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;//球体コンポーネント

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MoveComp;//モバイルコンポーネント

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectComp;//粒子コンポーネント

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UAudioComponent* AudioComp;//サウンドコンポーネント
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* ImpactVFX;//ビジュアルエフェクツ
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ImpactSound;
	
	//バーチャルなので、子クラスでこれをオーバーライドすることができます。
	UFUNCTION()
	virtual void OnActorHit( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	//BlueprintCallable蓝图能调用  BlueprintNativeEvent蓝图和C++能都能实现 C++调用蓝图外，同样会调用一个本地方法，本地方法为 声明的函数名+_Implementation
	//BlueprintNativeEvent = C++ base implementation, can be expnaded in blueprints
	//BlueprintCallable to allow child classes to trigger explosions
	//Not required for assignment, useful for expanding in Blueprint later on
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void Explode();

	virtual void PostInitializeComponents() override;
};
//Abstract标记指明UGameInstanceSubsystem是抽象基类，是不能被直接创建出来的。需要创建子类，子类可以创建实例。