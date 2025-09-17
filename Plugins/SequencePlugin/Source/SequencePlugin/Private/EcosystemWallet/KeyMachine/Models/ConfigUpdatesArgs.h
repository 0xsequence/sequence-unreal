#pragma once

#include "CoreMinimal.h"
#include "ConfigUpdatesArgs.generated.h"

USTRUCT()
struct FConfigUpdatesArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Wallet;

	UPROPERTY()
	FString FromImageHash;
};
