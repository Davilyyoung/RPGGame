#include "SCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"



ASCharacter::ASCharacter()
{
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	// We control the rotation of spring arm with pawn control rotation already, this disables a subtle side effect
	// where rotating our CapsuleComponent (eg. caused by bOrientRotationToMovement in Character Movement) will rotate our spring arm until it self corrects later in the update
	// This may cause unwanted effects when using CameraLocation during Tick as it may be slightly offset from our final camera position.
	SpringArmComp->SetUsingAbsoluteRotation(true);

	//初始化相机组件
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	//初始化装上这个组件 交互
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>(TEXT("InteractionComp"));

	//初始化装上这个组件 属性
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("AttributeComp"));

	//初始化装上这个组件 Action
	ActionComp = CreateDefaultSubobject<USActionComponent>(TEXT("ActionComp"));
	

	//旋转后角色都会是角色正面
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//视角可随意旋转
	bUseControllerRotationYaw = false;
	
	// Enabled on mesh to react to incoming projectiles
	GetMesh()->SetGenerateOverlapEvents(true);

	// Disable on capsule collision to avoid double-dipping and receiving 2 overlaps when entering trigger zones etc.
	// Once from the mesh, and 2nd time from capsule
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	// 受击闪烁
	TimeToHitParamName = "TimeToHit";
	
}

void ASCharacter::PostInitializeComponents()// Actor 的组件初始化后调用
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this,&ASCharacter::OnHealthChanged);//绑定这个动态多播
	
}


FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	SPlayerController = Cast<ASPlayerController>(GetController());

}


void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForWard",this,&ASCharacter::MoveForWard);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn",this,&APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp",this,&APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ASCharacter::Jump);

	PlayerInputComponent->BindAction("Sprint",IE_Pressed,this,&ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint",IE_Released,this,&ASCharacter::SprintStop);
	
	PlayerInputComponent->BindAction("PrimaryAttack",IE_Pressed,this,&ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Dash",IE_Pressed,this,&ASCharacter::Dash);
	PlayerInputComponent->BindAction("Ultimate",IE_Pressed,this,&ASCharacter::Ultimate);
	
	PlayerInputComponent->BindAction("PrimaryIneract",IE_Pressed,this,&ASCharacter::PrimaryInteract);
	
}

void ASCharacter::HealSelf(float Amount/* = 100 */)
{
	
	AttributeComp->ApplyHealthChange(this, Amount);
}

void ASCharacter::MoveForWard(float Value)
{
	//任务朝着摄像机正前方移动
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(),Value);
}

void ASCharacter::MoveRight(float Value)
{
	//左右移动不会原地打转
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
	AddMovementInput(RightVector,Value);
}

void ASCharacter::SprintStart()// 启动冲刺
{
	ActionComp->StartActionByName(this,"Sprint");
}

void ASCharacter::SprintStop()// 停止冲刺
{
	ActionComp->StopActionByName(this,"Sprint");
}

void ASCharacter::PrimaryAttack()//攻击技能
{
	ActionComp->StartActionByName(this,"PrimaryAttack");
}

void ASCharacter::Dash()//闪烁技能
{
	ActionComp->StartActionByName(this,"Dash");
}

void ASCharacter::Ultimate()//黑洞技能
{
	ActionComp->StartActionByName(this,"Ultimate");
}

void ASCharacter::PrimaryInteract()//交互
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	// Damaged
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// Rage added equal to damage received (Abs to turn into positive rage number)
		float RageDelta = FMath::Abs(Delta);
		AttributeComp->ApplyRage(InstigatorActor, RageDelta);
	}

	// Died
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(5.0f);
	}
}


