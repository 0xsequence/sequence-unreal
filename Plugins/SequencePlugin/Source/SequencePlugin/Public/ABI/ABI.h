// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "ABIElement.h"
#include "Types/BinaryData.h"

static constexpr int GBlockByteLength = 32;
static constexpr int GSignatureLength = 4;

class SEQUENCEPLUGIN_API ABI
{
public:
	static FUnsizedData Encode(FString Signature, TArray<TSharedPtr<ABIElement>> Args);
	static void Decode(TArray<uint8> Data, TArray<TSharedPtr<ABIElement>> Args);
	
	static TFixedABIData UInt32(uint32 Input = 0);
	static TFixedABIData Int32(int32 Input = 0);
	static TFixedABIData Bool(bool Input = false);
	static TFixedABIData Address(FAddress Address = FAddress::New());
	static TDynamicABIData String(FString Input = "");
	static FString Display(FString Signature, TArray<TSharedPtr<ABIElement>> Arr);

	// Deprecated functions
	static FUnsizedData Encode(FString Signature, TArray<ABIElement*> Args);
};