#pragma once

#include "CoreMinimal.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "SPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "SCharacter.generated.h"


UCLASS()
class RPGGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly)
	ASPlayerController* SPlayerController;

	// 受击闪烁
	UPROPERTY(EditDefaultsOnly,Category="Attack")
	FName TimeToHitParamName;
	
	UPROPERTY(EditDefaultsOnly,Category="Attack")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;
	
     //拿到属性组件health
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USAttributeComponent* AttributeComp;
	//玩家摄像机
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UCameraComponent* CameraComp;
	//摄像机吊臂
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USpringArmComponent* SpringArmComp;
	//使用接口组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USInteractionComponent* InteractionComp;
	//使用接口组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	USActionComponent* ActionComp;
	
	UFUNCTION()
	void OnHealthChanged(AActor* InsigatorActor,USAttributeComponent* OwningComp,float NewHealth,float Delta);//当血量法神变化
	
	void MoveForWard(float Value);
	
	void MoveRight(float Value);
	
	void SprintStart();	// 启动冲刺

	void SprintStop();	// 停止冲刺
	
	void PrimaryInteract(); // 交互
	
	void PrimaryAttack();	// 攻击技能
	
	void Dash();		// 传送技能

	void BlackHole();	// 黑洞技能
	
	virtual FVector GetPawnViewLocation() const override;// 返回角色的眼球位置  修复和物体交互的bug
	
	virtual void PostInitializeComponents() override;// 绑定事件在这里面绑定
	
	virtual void BeginPlay() override; //游戏开始时

public:
	
	ASCharacter();//构造

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;//动作绑定事件在这里
	
	UFUNCTION(Exec)//Exec制台命令 作弊
	void HealSelf(float Amount = 100.0f);
	
	

};
