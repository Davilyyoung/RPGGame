// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/SWorldUserWidget.h"
#include "SInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGGAME_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	void PrimaryInteract();

protected:

	// Reliable - Will always arrive, eventually. Request will be re-sent unless an acknowledgment was received.可靠的 - 总是会到达，最终。除非收到确认函，否则将重新发送请求
	// Unreliable - Not guaranteed, packet can get lost and won't retry. 不可靠 - 不保证，数据包可能丢失，不会重试。
	UFUNCTION(Server,Reliable,WithValidation)
	void ServerInteract(AActor* InFocus);//互交调用
	void ServerInteract_Implementation(AActor* InFocus);//互交实施
	bool ServerInteract_Validate(AActor* InFocus);//互交校验

	
	void FindBestInteractable();
	
	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;

	virtual void BeginPlay() override;
	
public:


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	USInteractionComponent();
	
};
