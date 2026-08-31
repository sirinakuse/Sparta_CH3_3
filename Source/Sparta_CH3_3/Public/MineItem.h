#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class SPARTA_CH3_3_API AMineItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AMineItem();

	USphereComponent* ExplosionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;

	bool bHasExploded;
	FTimerHandle ExplosionTimerHandle;

	virtual void ActivateItem(AActor* Activator) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void Explode();
};
