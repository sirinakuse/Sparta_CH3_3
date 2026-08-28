#pragma once

//cpp파일은 삭제하지 않는 편이 좋다.
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)//현재 모듈 내에서만 접근하도록 제한한다.
class UItemInterface : public UInterface//리플렉션을 위한 인터페이스, 실제로 수정을 하지 않음
{
	GENERATED_BODY()
};

//실제 구현하는 부분
class SPARTA_CH3_3_API IItemInterface
{
	GENERATED_BODY()

public:
	// = 0; 순수 가상 함수로 인터페이스의 경우에는 반드시 이 함수를 사용하라고 만든 것이기 때문에 가상함수가 아닌 순수가상함수를 사용한다.
	//가상함수의 경우에는 override를 할 지 안 할지 정할 수 있기 때문.
	UFUNCTION()//리플렉션 시스템에 등록을 해줘야 엔진이 런타임동안 함수를 찾을 수 있다.
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComp,//오버랩이 발생한 자기 자신(스피어컴포넌트)
		AActor* OtherActor,//부딪힌 액터
		UPrimitiveComponent* OtherComp,//부딪힌 액터의 가장 처음이 되는 컴포넌트? 캐릭터로 따지면 캡슐 컴포넌트
		int32 OtherBodyIndex,//아래 세개는 물리엔진 관련
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
	//반드시 플레이어가 진입한다는 보장은 없음(확장성부분)
	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0;
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;//아이템 타입? 타입형을 알아낼때는 FName을 사용한다.???

};
