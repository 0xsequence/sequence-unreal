#include "Util/SequenceCoder.h"
#include "Eth/Crypto.h"

TArray<uint8> FSequenceCoder::KeccakHash(const TArray<uint8>& Data)
{
	FUnsizedData UnsizedData = FUnsizedData(Data);
	return *GetKeccakHash(UnsizedData).Arr.Get();
}
