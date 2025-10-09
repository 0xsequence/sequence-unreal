#include "Types/BigInt.h"
#include "EthAbi/EthAbiBridge.h"

FBigInt::FBigInt(const FString& Value)
{
	if (Value.IsEmpty())
	{
		this->Value = "0";
	}
	else
	{
		this->Value = Value;
	}
}

TArray<uint8> FBigInt::Encode() const
{
	return FEthAbiBridge::EncodeBigInteger(this->Value);
}

FBigInt FBigInt::FromHex(const FString& Value)
{
	return FBigInt(FEthAbiBridge::HexToBigIntString(Value));
}
