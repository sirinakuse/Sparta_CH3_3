#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor * Activator)
{
	Super::ActivateItem(Activator);
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->AddScore(PointValue);
				bool bWaveEnded = GameState->OnCoinCollected();
				if (!bWaveEnded)//끝나지 않았을 때는 삭제해야됨. 끝났을때는 삭제하면 안됨
				{
					DestroyItem();
				}
			}
		}
	}
}
