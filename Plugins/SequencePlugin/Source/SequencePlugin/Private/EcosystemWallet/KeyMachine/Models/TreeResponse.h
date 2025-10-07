#pragma once

#include "CoreMinimal.h"
#include "TreeResponse.generated.h"

USTRUCT()
struct FTreeResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	int32 Version;

	TSharedPtr<FJsonValue> Tree;
};
