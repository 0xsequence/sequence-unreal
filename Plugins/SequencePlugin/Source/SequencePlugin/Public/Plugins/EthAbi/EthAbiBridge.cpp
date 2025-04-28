// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "EthAbiBridge.h"

extern "Rust" {
	char* encodeFunctionCall(const char* abiJson, const char* functionName, const char* argsJson);
}

FString FEthAbiBridge::EncodeFunctionCall(const FString& Abi, const FString& FunctionName, const FString& Values)
{
	const char* EncodedData = encodeFunctionCall(
		TCHAR_TO_UTF8(*Abi),
		TCHAR_TO_UTF8(*FunctionName),
		TCHAR_TO_UTF8(*Values));
	
	FString EncodedHex(UTF8_TO_TCHAR(EncodedData));
	return EncodedHex;
}

