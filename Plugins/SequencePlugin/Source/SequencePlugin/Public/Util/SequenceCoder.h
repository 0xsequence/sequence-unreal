#pragma once

class SEQUENCEPLUGIN_API FSequenceCoder
{
public:
	static TArray<uint8> KeccakHash(const TArray<uint8>& Data);
};
