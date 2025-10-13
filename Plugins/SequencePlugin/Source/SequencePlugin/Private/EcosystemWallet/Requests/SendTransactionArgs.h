#pragma once

#include "CoreMinimal.h"
#include "SendTransactionArgs.generated.h"

USTRUCT()
struct FTransactionRequest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString To;

	UPROPERTY()
	FString Data;
	
	UPROPERTY()
	FString Value;
	
	UPROPERTY()
	FString GasLimit;
};

USTRUCT()
struct FSendTransactionArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	int64 ChainId;
	
	UPROPERTY()
	FString Address;

	UPROPERTY()
	FTransactionRequest TransactionRequest;

	bool CustomJson = false;

	FString ToJson() const { return ""; }
};
