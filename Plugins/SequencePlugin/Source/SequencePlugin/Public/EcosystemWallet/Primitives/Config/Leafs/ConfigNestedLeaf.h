#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"
#include "EcosystemWallet/Primitives/Config/ConfigTopology.h"
#include "Util/ValueUtils.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FConfigNestedLeaf final : public FConfigLeaf
{
public:
	FConfigNestedLeaf(const FString& Weight, const FString& Threshold, const TSharedPtr<FConfigTopology>& Tree)
	{
		this->Type = EConfigLeafType::Nested;
		this->Weight = Weight;
		this->Threshold = Threshold;
		this->Tree = Tree;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		TArray<uint8> Nested = Tree->Encode(NoChainId, CheckpointerData);

		int32 Flag = FConfigTopology::FlagNested << 4;

		int32 WeightInt = FValueUtils::StringToInt32(Weight);
		TArray<uint8> WeightBytes;
		
		if (WeightInt > 0 && WeightInt <= 3)
		{
			Flag |= WeightInt << 2;
		}
		else if (WeightInt <= 255)
		{
			WeightBytes = FByteArrayUtils::ByteArrayFromNumber(WeightInt, FByteArrayUtils::MinBytesFor(WeightInt));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weight too large"));
		}

		int32 ThresholdInt = FValueUtils::StringToInt32(Threshold);
		TArray<uint8> ThresholdBytes;
		
		if (ThresholdInt > 0 && ThresholdInt <= 3)
		{
			Flag |= ThresholdInt;
		}
		else if (ThresholdInt <= 65535)
		{
			ThresholdBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::ByteArrayFromNumber(ThresholdInt, FByteArrayUtils::MinBytesFor(ThresholdInt)), 2);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Threshold too large"));
		}

		if (Nested.Num() > 0xFFFFFF)
		{
			UE_LOG(LogTemp, Error, TEXT("Nested tree too large"));
		}

		const int32 NestedSize = Nested.Num();
		TArray<uint8> NestedSizeBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::ByteArrayFromNumber(NestedSize, 1), 3);
		TArray<uint8> FlagBytes = FByteArrayUtils::ByteArrayFromNumber(Flag, 1);

		return FByteArrayUtils::ConcatBytes({WeightBytes, ThresholdBytes, NestedSizeBytes, Nested});
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}

	FString Weight;
	FString Threshold;
	TSharedPtr<FConfigTopology> Tree;
};
