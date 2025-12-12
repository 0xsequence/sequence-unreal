#pragma once

#include "CoreMinimal.h"
#include "ConfigUpdatesResponse.generated.h"

USTRUCT()
struct FConfigUpdate
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString ToImageHash;

	UPROPERTY()
	FString Signature;
};

USTRUCT()
struct FConfigUpdatesResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	TArray<FConfigUpdate> Updates;
};
