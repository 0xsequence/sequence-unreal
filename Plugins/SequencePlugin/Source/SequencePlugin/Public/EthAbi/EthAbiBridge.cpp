#include "EthAbiBridge.h"
#include "Containers/StringConv.h"

extern "C" {
	char* encode_function_call(const char* signature, const char* args_json);
	char* decode_function_result(const char* abi_json, const char* encoded_data);
	void free_string(char* ptr);
}

FString FEthAbiBridge::EncodeFunctionCall(const FString& FunctionSignature, const FString& Values)
{
	const char* EncodedData = encode_function_call(
		TCHAR_TO_UTF8(*FunctionSignature),
		TCHAR_TO_UTF8(*Values));
	
	FString EncodedHex(UTF8_TO_TCHAR(EncodedData));
	return EncodedHex;
}

FString FEthAbiBridge::DecodeFunctionResult(const FString& Abi, const FString& EncodedData)
{
	char* ResultPtr = decode_function_result(TCHAR_TO_UTF8(*Abi), TCHAR_TO_UTF8(*EncodedData));
	if (ResultPtr != nullptr) {
		FString JsonResult(UTF8_TO_TCHAR(ResultPtr));
		free_string(ResultPtr);
		return JsonResult;
	}

	return "";
}
