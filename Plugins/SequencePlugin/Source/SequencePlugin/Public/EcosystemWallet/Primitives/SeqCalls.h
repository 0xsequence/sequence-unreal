#pragma once

#include "SeqCall.h"
#include "EcosystemWallet/SeqCalls.generated.h"

USTRUCT()
struct FSeqCalls
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Type;
	
	UPROPERTY()
	int64 Space;

	UPROPERTY()
	int64 Nonce;

	UPROPERTY()
	TArray<FSeqCall> Calls;
};
