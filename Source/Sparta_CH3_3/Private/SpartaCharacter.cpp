#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;//카메라와 타겟과의 거리
	SpringArmComp->bUsePawnControlRotation = true;//마우스 시점회전을 미리 설정해두는 것. 시점 움직일때 스프링 암도 같이 회전하게 해주는 것.

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);//소켓중 끝부분에 붙여줘야함 USpringArmComponent::SocketName == 소켓 끝 부분
	CameraComp->bUsePawnControlRotation = false;//카메라는 움직이지 않도록 꺼준다.

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(GetMesh());
	OverHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;//기본 걷는 속도

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverHeadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)//콜백함수. 캐릭터를 조종할 준비가 됐을 때, 입력을 어떻게 처리할지 여기서 설정한다.
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		//캐스팅? 입력방식이 다양하게 있기 때문에 Enhanced를 사용하겠다고 지정해주는 작업(BP노드로 따지면 Cast To[클래스 이름])
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		//현재 캐릭터를 조작하는 컨트롤러를 GetController를 가져와서 ASpartaPlayerController로 캐스팅? 해준다. 
		{
			if (PlayerController->MoveAction)//null인지 아닌지 파악하고 바인딩을 해준다. 안전장치
			{
				EnhancedInput->BindAction(//MoveAction이 있을 때 연결을 시키겠다.
					PlayerController->MoveAction,//IA_Move를 가져온다.
					ETriggerEvent::Triggered,//키가 눌려서 이벤트가 발생했을 때
					this,//입력 이벤트가 발생했을때 객체
					&ASpartaCharacter::Move//실제 호출된 함수
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;// 컨트롤러가 실제로 유요한지 확인용. 안전장치

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))// 부동소수점 오차문제 안전장치
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);// 캐릭터 무브먼트 컴포넌트
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))// 부동소수점 오차문제 안전장치
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())//true일때
	{
		Jump();//엔진 내부에 구현되있는 함수
	}
}
void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())//false일때
	{
		StopJumping();
	}
}
void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}
void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}
void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverHeadHP();
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	//실제 데미지
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverHeadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverHeadHP()
{
	if (!OverHeadWidget) return;

	UUserWidget* OverHeadWidgetInstance = OverHeadWidget->GetUserWidgetObject();
	if (!OverHeadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverHeadWidgetInstance->GetWidgetFromName("OverHeadHP")))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}
