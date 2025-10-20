#pragma once

#include "CoreMinimal.h"

struct SEQUENCEPLUGIN_API FBigInt
{
public:
	explicit FBigInt(const FString& Value);

	TArray<uint8> Encode() const;

	void Add(const FBigInt& InBigInt);

	static FBigInt FromHex(const FString& Value);

	FString Value;
};