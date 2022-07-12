// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;

	SweepRadius = 20.0f;
	SweepDistanceFallback = 5000;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackMontage);

		UGameplayStatics::SpawnEmitterAttached(CastingEffect,Character->GetMesh(),HandSocketName,FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget);
		
		UGameplayStatics::SpawnSoundAttached(CastingSound, Character->GetMesh());
		
		if (Character->HasAuthority())//实在服务器上运行这个逻辑？
		{
			//如果是我们才设置这个魔法弹
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;//本地或动态的定时器委托
			Delegate.BindUFunction(this,"AttackDelay_Elapsed",Character);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay,Delegate,AttackAnimDelay,false);
		}
	
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		//选择骨骼
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		// We trace against the environment first to find whats under the player crosshair.
		// We use the hit location to adjust the projectile launch direction so it will hit what is under the crosshair rather than shoot straight forward from the player hands.

		//传递给SpawnActor函数的可选参数的结构。
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(SweepRadius);
		
		//忽略我们自己
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FVector TraceDirection = InstigatorCharacter->GetControlRotation().Vector();
		// Add sweep radius onto start to avoid the sphere clipping into floor/walls the camera is directly against.
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation() + (TraceDirection * SweepRadius);
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		FVector TraceEnd = TraceStart + (TraceDirection * SweepDistanceFallback);

		FHitResult Hit;
		// returns true if we got to a blocking hit (Channel1="Projectile" defined in DefaultGame.ini)
		if (GetWorld()->SweepSingleByObjectType(Hit,TraceStart,TraceEnd,FQuat::Identity,ECC_GameTraceChannel1,Shape,Params))
		{
			// Overwrite trace end with impact point in world
			TraceEnd = Hit.ImpactPoint;
		}
		
		// find new direction/rotation from Hand pointing to impact point in world.
		FRotator ProjRotation = (TraceEnd - HandLocation).Rotation();

		FTransform SpawnTM = FTransform(ProjRotation,HandLocation);
		
		
		GetWorld()->SpawnActor<AActor>(ProjectileClass,SpawnTM,SpawnParams);
		
	}

	StopAction(InstigatorCharacter);
}

