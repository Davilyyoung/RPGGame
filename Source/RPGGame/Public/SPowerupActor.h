#pragma once

#include "CoreMinimal.h"
#include "SGamePlayInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class RPGGAME_API ASPowerupActor : public AActor,public ISGamePlayInterface
{
	GENERATED_BODY()
	
public:	
	
	ASPowerupActor();
	
protected:

	UPROPERTY(ReplicatedUsing="OnRep_IsActive")
	bool bIsActive;

	UFUNCTION()
	void OnRep_IsActive();
	
	UPROPERTY(EditAnywhere,Category="Powerup")
	float RespawnTime;

	FTimerHandle TimerHandle_RespawnTimer;

	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);
	
	UPROPERTY(VisibleAnywhere,Category="Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere,Category="Components")
	UStaticMeshComponent* MeshComp;

public:
	void Interact_Implementation(APawn* InstigatorPawn) override;//继承接口互交

	FText GetInteractText_Implementation(APawn* InstigatorPawn);//继承接口 实现文本显示

};
