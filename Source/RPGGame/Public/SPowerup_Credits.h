
#pragma once

#include "CoreMinimal.h"
#include "SGamePlayInterface.h"
#include "SPowerupActor.h"
#include "SPowerup_Credits.generated.h"

UCLASS()
class RPGGAME_API ASPowerup_Credits : public ASPowerupActor 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerup_Credits();


protected:
	
	UPROPERTY(EditAnywhere, Category = "Credits")
	int32 CreditsAmount;
	//重写接口
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
	 
};
