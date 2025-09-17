#pragma once

#include "CoreMinimal.h"
#include "ConfigArgs.generated.h"

USTRUCT()
struct FConfigArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString ImageHash;
};
