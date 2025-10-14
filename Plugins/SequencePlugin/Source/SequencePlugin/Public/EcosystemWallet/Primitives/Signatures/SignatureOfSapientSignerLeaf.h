#pragma once
#include "SignatureOfLeaf.h"
#include "Util/ValueUtils.h"
#include "Util/ByteArrayUtils.h"

enum ESapientSignatureType
{
	SapientSignature,
	SapientSignatureCompact
};

class SEQUENCEPLUGIN_API FSignatureOfSapientSignerLeaf final : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSapientSignerLeaf(const ESapientSignatureType& Type, const FString& Address, const TArray<uint8>& Data)
	{
		this->Type = EConfigSignatureType::SignatureOfSapientSigner;
		this->SapientType = Type;
		this->Address = Address;
		this->Data = Data;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		const int32 WeightInt = FValueUtils::StringToInt32(Weight);
		TArray<uint8> WeightBytes;

		int32 FlagBase = SapientType == ESapientSignatureType::SapientSignature
			? FConfigTopology::FlagSignatureSapient
			: FConfigTopology::FlagSignatureSapientCompact;

		int32 Flag = FlagBase << 4;

		const int32 BytesForSignatureSize = FByteArrayUtils::MinBytesFor(Data.Num());
		if (BytesForSignatureSize > 3)
		{
			UE_LOG(LogTemp, Error, TEXT("Signature too large"));
		}
		Flag |= (BytesForSignatureSize << 2);

		if (WeightInt > 0 && WeightInt <= 3)
		{
			Flag |= WeightInt;
		}
		else if (WeightInt <= 255)
		{
			WeightBytes = FByteArrayUtils::ByteArrayFromNumber(WeightInt, FByteArrayUtils::MinBytesFor(WeightInt));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weight too large"));
		}

		TArray<uint8> FlagBytes = FByteArrayUtils::ByteArrayFromNumber(Flag, FByteArrayUtils::MinBytesFor(Flag));

		TArray<uint8> AddressBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(Address), 20);

		TArray<uint8> DataSizeBytes = FByteArrayUtils::ByteArrayFromNumber(Data.Num(), BytesForSignatureSize);

		return FByteArrayUtils::ConcatBytes({FlagBytes, WeightBytes, AddressBytes, DataSizeBytes, Data});
	}

	ESapientSignatureType SapientType;
	FString Address;
	TArray<uint8> Data;
};
