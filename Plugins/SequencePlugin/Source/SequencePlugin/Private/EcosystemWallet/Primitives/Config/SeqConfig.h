#pragma once

#include "CoreMinimal.h"
#include "ConfigTopology.h"
#include "UObject/Object.h"
#include "SeqConfig.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USeqConfig : public UObject
{
	GENERATED_BODY()
	
public:
	int64 Threshold;
	int64 Checkpointer;
	FConfigTopology* Topology;
	FString Address;
};
