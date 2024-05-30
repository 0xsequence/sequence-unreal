// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "ABIEncodeable.h"
#include "Types/BinaryData.h"

static constexpr int GBlockByteLength = 32;
static constexpr int GSignatureLength = 4;

class SEQUENCEPLUGIN_API ABI : public ABIEncodeable
{
public:
	static TFixedABIData UInt32(uint32 Input);
	static TFixedABIData Int32(int32 Input);
	static TFixedABIData Bool(bool Input);
	static TFixedABIData Address(FAddress Address);
	static TDynamicABIData String(FString Input);
	static FUnsizedData Encode(FString Signature, TArray<ABIEncodeable*> Arr);
	static FString Display(FString Signature, TArray<ABIEncodeable*> Arr);
};