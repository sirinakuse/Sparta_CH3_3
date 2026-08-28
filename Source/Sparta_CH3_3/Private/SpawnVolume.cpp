#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;//유효하지 않을경우 nullptr

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));

	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->Spawnchance;
		}
	}

	const float RandomValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;
	//누적 확률 랜덤 뽑기

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->Spawnchance;
		if (RandomValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const//랜덤좌표구하기
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	//박스 컴포넌트의 절반 크기
	//중심부터 끝까지의 크기
	FVector BoxOrigin = SpawningBox->GetComponentLocation();//컴포넌트 중심 위치값

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),//사이값을 아무거나 랜덤으로 가져온다. 중심으로부터 X값만큼의 길이 사이에서 랜덤으로 뽑기
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		50
	);
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass, //스폰할 액터? 클래스?
		GetRandomPointInVolume(),//스폰할 위치
		FRotator::ZeroRotator//회전값 ZeroRotator는 회전안하겠다는 뜻
	);

	return SpawnedActor;
}
