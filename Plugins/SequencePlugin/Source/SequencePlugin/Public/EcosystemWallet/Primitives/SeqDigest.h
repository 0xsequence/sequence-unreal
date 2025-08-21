#pragma once

#include "EcosystemWallet/SeqDigest.generated.h"

USTRUCT()
struct FSeqDigest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Type;

	UPROPERTY()
	FString Digest;
};
