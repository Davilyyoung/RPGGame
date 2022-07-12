// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "SActionComponent.h"
#include "SGamePlayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	
	SphereComp->SetSphereRadius(20.0f);
	InitialLifeSpan = 10.0f;
	DamageAmount = 20.0f;
	
}
void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// PostInitializeComponent是绑定任何事件的首选方式。
	// More consistent to bind here compared to Constructor which may fail to bind if Blueprint was created before adding this binding (or when using hotreload)
	// PostInitializeComponent is the preferred way of binding any events.
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

 void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
	
	if (OtherActor && OtherActor != GetInstigator())//反弹
	{
		//static FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Status.Parring");
		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));

		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MoveComp->Velocity = -MoveComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));

			return;
		}
		// Apply Damage & Impulse
		if (USGamePlayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			// We only explode if the target can be damaged, it ignores anything it Overlaps that it cannot Damage (it requires an AttributeComponent on the target)
			Explode();

			if (ActionComp && BurningActionClass && HasAuthority())
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
	

 }



