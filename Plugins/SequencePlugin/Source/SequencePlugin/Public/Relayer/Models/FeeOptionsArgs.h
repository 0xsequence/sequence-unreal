#pragma once

#include "CoreMinimal.h"
#include "FeeOptionsArgs.generated.h"

USTRUCT()
struct FFeeOptionsArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Wallet;
	
	UPROPERTY()
	FString To;
	
	UPROPERTY()
	FString Data;

	UPROPERTY()
	bool Simulate;
};
