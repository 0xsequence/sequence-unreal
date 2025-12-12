#pragma once

#include "CoreMinimal.h"
#include "DeployHashArgs.generated.h"

USTRUCT()
struct FDeployHashArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Wallet;
};
