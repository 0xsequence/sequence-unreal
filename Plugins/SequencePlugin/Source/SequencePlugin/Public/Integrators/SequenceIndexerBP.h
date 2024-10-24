// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequenceRPCManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Indexer/Structs/SeqGetTokenBalancesArgs.h"
#include "SequenceIndexerBP.generated.h"

UCLASS(Blueprintable)
class SEQUENCEPLUGIN_API USequenceIndexerBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USequenceIndexerBP();

	UFUNCTION(BlueprintCallable, Category="Sequence")
	void GetTokenBalancesAsync(const int64 ChainID, const FSeqGetTokenBalancesArgs& Args);
	
private:
	static USequenceIndexerBP* GetSubSystem();
};
