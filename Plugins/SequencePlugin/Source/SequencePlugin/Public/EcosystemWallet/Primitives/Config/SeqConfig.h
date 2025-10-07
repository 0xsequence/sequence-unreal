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
	FString Checkpoint;
	TSharedPtr<FConfigTopology> Topology;
	FString Address;
};
