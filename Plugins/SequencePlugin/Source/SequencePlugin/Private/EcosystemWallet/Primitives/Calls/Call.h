#pragma once

#include "CoreMinimal.h"
#include "Call.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FCall
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FString To;

	UPROPERTY()
	FString Data;
	
	UPROPERTY()
	int64 Value;

	UPROPERTY()
	int64 GasLimit;

	UPROPERTY()
	bool DelegateCall;
	
	UPROPERTY()
	bool OnlyFallback;
};
