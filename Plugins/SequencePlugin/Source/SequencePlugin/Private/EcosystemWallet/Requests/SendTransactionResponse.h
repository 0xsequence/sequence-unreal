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

	inline static bool CustomJson = false;

	static FSendTransactionResponse FromJson(const TSharedPtr<FJsonObject>& Json)
	{
		return FSendTransactionResponse();
	}
};
