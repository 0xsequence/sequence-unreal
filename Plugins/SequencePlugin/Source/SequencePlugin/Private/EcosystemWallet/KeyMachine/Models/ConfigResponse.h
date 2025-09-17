#pragma once

#include "CoreMinimal.h"
#include "ConfigResponse.generated.h"

USTRUCT()
struct FSeqConfigContext
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Checkpoint;
	
	UPROPERTY()
	int32 Threshold;
	
	UPROPERTY()
	FString Tree;
};

USTRUCT()
struct FConfigResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	int32 Version;
	
	UPROPERTY()
	FSeqConfigContext Config;
};
