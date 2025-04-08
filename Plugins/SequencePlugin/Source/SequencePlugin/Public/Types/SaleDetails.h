#pragma once

#include "BinaryData.h"
#include "SaleDetails.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSaleDetails
{
	GENERATED_USTRUCT_BODY()

public:
	FString PaymentToken;
	long Cost;
	long SupplyCap;
	long StartTimeLong;
	long EndTimeLong;
	long Amount;
	TArray<uint8> MerkleRoot;

	void Initialize(FUnsizedData Data);
};
