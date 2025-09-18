#pragma once

#include "CoreMinimal.h"
#include "IntentPrecondition.generated.h"

USTRUCT()
struct FIntentPrecondition
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Type;

	UPROPERTY()
	FString ChainId;
	
	UPROPERTY()
	FString Data;
};
