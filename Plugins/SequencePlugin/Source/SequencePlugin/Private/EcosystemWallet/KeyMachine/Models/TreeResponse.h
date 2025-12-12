#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonValue.h"
#include "TreeResponse.generated.h"

USTRUCT()
struct FTreeResponse
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Version;

	TSharedPtr<FJsonValue> Tree;
};
