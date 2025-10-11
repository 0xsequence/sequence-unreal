#pragma once

#include "CoreMinimal.h"
#include "ConfigTopology.h"
#include "UObject/Object.h"

class SEQUENCEPLUGIN_API FSeqConfig
{
public:
	int64 Threshold;
	FString Checkpoint;
	TSharedPtr<FConfigTopology> Topology;
	TOptional<FString> Checkpointer;
};
