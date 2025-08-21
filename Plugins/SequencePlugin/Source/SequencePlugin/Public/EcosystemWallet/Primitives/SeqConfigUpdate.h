#pragma once

#include "EcosystemWallet/SeqConfigUpdate.generated.h"

USTRUCT()
struct FSeqConfigUpdate
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Type;

	UPROPERTY()
	FString ImageHash;
};
