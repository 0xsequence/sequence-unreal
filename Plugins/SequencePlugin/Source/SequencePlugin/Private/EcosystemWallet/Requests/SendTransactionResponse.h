#pragma once

#include "CoreMinimal.h"
#include "SendTransactionResponse.generated.h"

USTRUCT()
struct FSendTransactionResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString TransactionHash;

	UPROPERTY()
	FString WalletAddress;
};
