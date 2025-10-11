#pragma once

enum ESessionCallSignatureType
{
	Implicit,
	Explicit,
};

class SEQUENCEPLUGIN_API FSessionCallSignature
{
public:
	virtual ~FSessionCallSignature() = default;
	
	static TArray<uint8> EncodeSignatures();

	ESessionCallSignatureType Type;
};
