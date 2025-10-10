#pragma once

class SEQUENCEPLUGIN_API FRawSignature
{
public:
	TArray<uint8> Encode(bool SkipCheckpointerData = false, bool SkipCheckpointerAddress = false);

	static TSharedPtr<FRawSignature> Decode(const TArray<uint8>& Erc6492Signature);
};
