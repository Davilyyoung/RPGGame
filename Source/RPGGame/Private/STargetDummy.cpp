// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));
	AttributeComp-> OnHealthChanged.AddDynamic(this,&ASTargetDummy::OnHealthChanged);//绑定这个动态多播
}

void ASTargetDummy::OnHealthChanged(AActor* InsigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit",GetWorld()->TimeSeconds);
	}
	
}



