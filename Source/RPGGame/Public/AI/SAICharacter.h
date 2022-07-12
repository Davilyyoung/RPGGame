
#pragma once

#include "CoreMinimal.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "UI/SWorldUserWidget.h"
#include "SAICharacter.generated.h"


UCLASS()
class RPGGAME_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();
	

protected:
	
	UPROPERTY(EditDefaultsOnly,Category="UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	/* Widget to display when bot first sees a player. */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;
	
	//拿到属性组件health
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USAttributeComponent* AttributeComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USActionComponent* ActionComp;

	
	UPROPERTY()
	USWorldUserWidget* ActiveHealthBar;

	
	
	/* Key for AI Blackboard 'TargetActor' */
	UPROPERTY(VisibleAnywhere, Category = "AI")
	FName TargetActorKey;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	FName TimeToHitParamName;


	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);
	
	UFUNCTION()
	void OnHealthChanged(AActor* InsigatorActor,USAttributeComponent* OwningComp,float NewHealth,float Delta);
	
	virtual void PostInitializeComponents() override;

};
