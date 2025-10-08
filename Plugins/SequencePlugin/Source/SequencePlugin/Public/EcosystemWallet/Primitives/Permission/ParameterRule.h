#pragma once

#include "CoreMinimal.h"
#include "Util/ByteArrayUtils.h"
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
	FString Offset;
	
	UPROPERTY()
	TArray<uint8> Value;
	
	UPROPERTY()
	TArray<uint8> Mask;
	
	static bool Decode(const TArray<uint8>& Data, FParameterRule& OutRule)
	{
		if (Data.Num() != 97)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid parameter rule length"));
			return false;
		}

		uint8 OperationCumulative = Data[0];
		bool bCumulative = (OperationCumulative & 1) == 1;
		int32 Operation = OperationCumulative >> 1;

		// --- Value (bytes 1–32) ---
		TArray<uint8> Value;
		Value.Append(&Data[1], 32);

		// --- Offset (bytes 33–64) as BigInt hex string ---
		TArray<uint8> OffsetBytes;
		OffsetBytes.Append(&Data[33], 32);
		FString OffsetHex = FByteArrayUtils::BytesToBigIntHexString(OffsetBytes);

		// --- Mask (bytes 65–96) ---
		TArray<uint8> Mask;
		Mask.Append(&Data[65], 32);

		OutRule.Cumulative = bCumulative;
		OutRule.Operation = Operation;
		OutRule.Value = Value;
		OutRule.Offset = OffsetHex;
		OutRule.Mask = Mask;

		return true;
	}
};
