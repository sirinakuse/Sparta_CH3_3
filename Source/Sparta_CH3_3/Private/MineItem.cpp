#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ExplosionDelay = 5.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 30;
	ItemType = "Mine";
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);
	//타이머 핸들러, 타이머 매니저
	if (!GetWorldTimerManager().IsTimerActive(ExplosionTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			ExplosionTimerHandle,
			this,//개체
			&AMineItem::Explode,//호출 할 함수
			ExplosionDelay,//시간
			false//반복여부
		);
		SetActorTickEnabled(true);
	}
	bHasExploded = true;
}

void AMineItem::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = StaticMesh->CreateDynamicMaterialInstance(0);
}

void AMineItem::Tick(float DeltaTime)
{
	float RemainingTime = GetWorldTimerManager().GetTimerRemaining(ExplosionTimerHandle);
	RemainingTime = FMath::Max(RemainingTime, 0.0f);

	float BlinkSpeed =10.0f / RemainingTime;
	float SinValue = FMath::Sin(GetWorld()->GetTimeSeconds() * BlinkSpeed);
	float GlowIntensity = (SinValue + 1.0f) * 0.5f;

	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("PulseColor"), BaseGlowColor * GlowIntensity);
	}
}

void AMineItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(ExplosionTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void AMineItem::Explode()
{
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}

	DestroyItem();
}
