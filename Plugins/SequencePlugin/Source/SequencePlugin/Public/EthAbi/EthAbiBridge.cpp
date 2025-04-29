// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "EthAbiBridge.h"

#include "Containers/StringConv.h"

extern "C" {
	char* encode_function_call(const char* abi_json, const char* function_name, const char* args_json);
}

FString FEthAbiBridge::EncodeFunctionCall(const FString& Abi, const FString& FunctionName, const FString& Values)
{
	const char* EncodedData = encode_function_call(
		TCHAR_TO_UTF8(*Abi),
		TCHAR_TO_UTF8(*FunctionName),
		TCHAR_TO_UTF8(*Values));
	
	FString EncodedHex(UTF8_TO_TCHAR(EncodedData));
	return EncodedHex;
}

