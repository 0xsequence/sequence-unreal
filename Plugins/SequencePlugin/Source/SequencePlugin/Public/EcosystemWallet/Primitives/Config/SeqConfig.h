#pragma once

#include "CoreMinimal.h"
#include "ConfigTopology.h"
#include "Types/BigInt.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FSeqConfig
{
public:
	explicit FSeqConfig(const FBigInt& Threshold, const FBigInt& Checkpoint, const TSharedPtr<FConfigTopology>& Topology)
		: Threshold(Threshold), Checkpoint(Checkpoint), Topology(Topology), Checkpointer("") { }
	
	FBigInt Threshold;
	FBigInt Checkpoint;
	TSharedPtr<FConfigTopology> Topology;
	TOptional<FString> Checkpointer;
};
