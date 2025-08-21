#pragma once

#include "EcosystemWallet/SeqMessage.generated.h"

USTRUCT()
struct FSeqMessage
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString Type;

	UPROPERTY()
	TArray<uint8> Message;
};
