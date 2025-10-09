#pragma once

struct SEQUENCEPLUGIN_API FBigInt
{
public:
	explicit FBigInt(const FString& Value);

	TArray<uint8> Encode() const;

	static FBigInt FromHex(const FString& Value);

	FString Value;
};