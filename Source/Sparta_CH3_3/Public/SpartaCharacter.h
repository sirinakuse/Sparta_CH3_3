#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;//미리 선언
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class SPARTA_CH3_3_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()
	//스크린 모드 - 항상 정면에서 보임
	//월드모드 - 캐릭터의 움직임에 따라 돌아감

public:
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")//객체 자체를 변경하는 건 불가능하지만 내부속성은 변경이 가능함
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverHeadWidget;

	UFUNCTION(BlueprintPure, Category = "health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "health")
	void AddHealth(float Amount);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float Health;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;//IA 직접 연결해주는 장소
	virtual float TakeDamage(
		float DamageAmount,//기본적인 데미지 양
		struct FDamageEvent const& DamageEvent,//데미지를 받은 유형, 추가적인 정보, 예시)스킬시스템-스킬에 따라 어떤 데미지? 여기선 null
		AController* EventInstigator,//데미지를 누가 입혔는지. 지뢰를 심은 사람? 이 플젝에선 null
		//데미지를 일으킨 오브젝트
		AActor* DamageCauser) override;

	virtual void BeginPlay() override;

	//블루프린트에 노출을 하지 않아도 리플렉션을 해주는게 좋다
	UFUNCTION()
	void Move(const FInputActionValue& value);//복사비용을 줄이고 객체 수정을 못하게 막기위한 형태.
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverHeadHP();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;//얼마나 곱할지
	float SprintSpeed;//얼마나 빨라졌는지
};
