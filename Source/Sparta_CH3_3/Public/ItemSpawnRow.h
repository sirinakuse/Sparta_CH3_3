#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase//FTableRowBase 데이터 테이블 행으로 쓸 수 있다?
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;//아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ItemClass;//아이템 클래스 하드 레퍼런스 TSoftClassPtr - 소프트 레퍼런스
	//하드 레퍼런스: 클래스가 항상 메모리에 로드된 상태에서 바로 접근
	//소프트 레퍼런스: 클래스의 경로만 유지. 클래스가 필요한 상황이 되면 그때 메모리에 로드한다.
	//기본적으로 소프트 레퍼런스를 권장한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Spawnchance = 0.0f;//확률
};
