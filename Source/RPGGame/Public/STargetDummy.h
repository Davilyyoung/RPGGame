#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "STargetDummy.generated.h"

UCLASS()
class RPGGAME_API ASTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTargetDummy();

protected:

	UPROPERTY(VisibleAnywhere)
	USAttributeComponent* AttributeComp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;
	UFUNCTION()
	void OnHealthChanged(AActor* InsigatorActor,USAttributeComponent* OwningComp,float NewHealth,float Delta);
	
};
