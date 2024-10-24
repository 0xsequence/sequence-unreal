
#include "Integrators/SequenceIndexerBP.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Util/Log.h"

USequenceIndexerBP::USequenceIndexerBP()
{
	
}

USequenceIndexerBP* USequenceIndexerBP::GetSubSystem()
{
	if (GEngine)
	{
		const TIndirectArray<FWorldContext> Contexts = GEngine->GetWorldContexts();
		for (FWorldContext Context : Contexts)
		{
			if (const UWorld* World = Context.World())
			{
				if (const UGameInstance* GI = UGameplayStatics::GetGameInstance(World))
				{
					if (USequenceIndexerBP* Subsystem = GI->GetSubsystem<USequenceIndexerBP>())
					{
						return Subsystem;
					}
				}
			}
		}
	}
	else
	{
		SEQ_LOG(Error,TEXT("Error Accessing GEngine"));
	}
	
	SEQ_LOG(Error,TEXT("Error Accessing USequenceWallet GameInstanceSubSystem"));
	return nullptr;
}

void USequenceIndexerBP::GetTokenBalancesAsync(const int64 ChainID, const FSeqGetTokenBalancesArgs& Args)
{
	
}
