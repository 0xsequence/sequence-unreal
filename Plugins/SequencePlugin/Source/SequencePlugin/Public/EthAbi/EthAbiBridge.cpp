// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "EthAbiBridge.h"

#include "Containers/StringConv.h"

extern "C" {
	char* encode_function_call(const char* abi_json, const char* function_name, const char* args_json);
	char* decode_function_result(const char* abi_json, const char* encoded_data);
	void free_string(char* ptr);
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

FString FEthAbiBridge::DecodeFunctionResult(const FString& Abi, const FString& EncodedData)
{
	char* resultPtr = decode_function_result(TCHAR_TO_UTF8(*Abi), TCHAR_TO_UTF8(*EncodedData));

	if (resultPtr != nullptr) {
		FString JsonResult(UTF8_TO_TCHAR(resultPtr));
		free_string(resultPtr);
		return JsonResult;
	}

	return "";
}
