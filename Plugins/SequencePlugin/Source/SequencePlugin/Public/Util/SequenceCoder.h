#pragma once

class SEQUENCEPLUGIN_API FSequenceCoder
{
public:
	static TArray<uint8> KeccakHash(const TArray<uint8>& Data);

	static TArray<uint8> EncodeAddress(const FString& Value);
	
	static TArray<uint8> EncodeSignedInt(const FString& Value);

	static TArray<uint8> EncodeBoolean(const bool& Value);
};
