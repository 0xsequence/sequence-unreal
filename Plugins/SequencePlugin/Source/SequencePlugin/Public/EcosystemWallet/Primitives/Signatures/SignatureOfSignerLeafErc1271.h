#pragma once
#include "SignatureOfLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Util/ValueUtils.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafErc1271 : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafErc1271(const FString& Address, const TArray<uint8>& Data)
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
		this->Address = Address;
		this->Data = Data;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		const int32 WeightInt = FValueUtils::StringToInt32(Weight);
		int32 Flag = FConfigTopology::FlagSignatureErc1271 << 4;
		TArray<uint8> WeightBytes;

		// --- 2. Data size field (sizeLen) ---
		const int32 SizeLen = FByteArrayUtils::MinBytesFor(Data.Num());
		if (SizeLen > 3)
		{
			UE_LOG(LogTemp, Error, TEXT("Signature too large"));
		}
		
		Flag |= (SizeLen << 2);

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
			checkNoEntry();
		}

		TArray<uint8> FlagBytes = FByteArrayUtils::ByteArrayFromNumber(Flag, FByteArrayUtils::MinBytesFor(Flag));

		TArray<uint8> AddressBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(Address), 20);

		TArray<uint8> DataLenBytes = FByteArrayUtils::ByteArrayFromNumber(Data.Num(), SizeLen);

		return FByteArrayUtils::ConcatBytes({FlagBytes, AddressBytes, DataLenBytes, Data});
	}

	FString Address;
	TArray<uint8> Data;
};
