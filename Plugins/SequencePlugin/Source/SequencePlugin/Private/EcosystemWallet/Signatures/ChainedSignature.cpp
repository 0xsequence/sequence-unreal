#include "ChainedSignature.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FChainedSignature::Encode()
{
	if (Signatures.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Signatures list cannot be empty."));
		checkNoEntry();
	}

	uint8 Flag = 0x01;

	const auto& SigForCheckpointer = Signatures.Last();
	if (SigForCheckpointer.Configuration->Checkpointer.IsSet())
	{
		Flag |= 0x40;
	}

	TArray<uint8> Output;
	Output.Add(Flag);

	TArray<uint8> CheckpointerBytes;
	if (SigForCheckpointer.Configuration->Checkpointer.IsSet() &&
		FByteArrayUtils::HexStringToBytes(SigForCheckpointer.Configuration->Checkpointer.GetValue(), CheckpointerBytes))
	{
		TArray<uint8> Checkpointer = FByteArrayUtils::PadLeft(CheckpointerBytes, 20);
		Output.Append(Checkpointer);

		int32 CheckpointerDataSize = SigForCheckpointer.CheckpointerData.Num();
		if (CheckpointerDataSize > 16777215)
		{
			UE_LOG(LogTemp, Error, TEXT("Checkpointer data too large"));
			checkNoEntry();
		}

		Output.Append(FByteArrayUtils::ByteArrayFromNumber(CheckpointerDataSize, 3));
		if (CheckpointerDataSize > 0)
			Output.Append(SigForCheckpointer.CheckpointerData);
	}

	for (int32 i = 0; i < Signatures.Num(); i++)
	{
		FRawSignature Sig = Signatures[i];
		bool bIsLast = (i == Signatures.Num() - 1);

		TArray<uint8> Encoded = Sig.Encode(true, bIsLast);
		if (Encoded.Num() > 16777215)
		{
			UE_LOG(LogTemp, Error, TEXT("Chained signature too large"));
			checkNoEntry();
		}

		TArray<uint8> EncodedSize = FByteArrayUtils::ByteArrayFromNumber(Encoded.Num(), 3);
		Output.Append(EncodedSize);
		Output.Append(Encoded);
	}

	return Output;
}
