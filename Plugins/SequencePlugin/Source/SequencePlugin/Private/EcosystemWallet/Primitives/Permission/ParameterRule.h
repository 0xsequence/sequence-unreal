#pragma once

#include "CoreMinimal.h"
#include "ParameterRule.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FParameterRule
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	bool Cumulative;
	
	UPROPERTY()
	int32 Operation;
	
	UPROPERTY()
	int64 Offset;
	
	UPROPERTY()
	TArray<uint8> Value;
	
	UPROPERTY()
	TArray<uint8> Mask;
};
