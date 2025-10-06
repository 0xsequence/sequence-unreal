#include "EthAbiBridge.h"
#include "Containers/StringConv.h"

extern "C" {
	char* encode_function_call(const char* signature, const char* args_json);
	char* decode_function_result(const char* abi_json, const char* encoded_data);
	char* encode_bigint_to_bytes(const char* num_str, int* out_len);
	void free_encoded_bytes(char* ptr);
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

TArray<uint8> FEthAbiBridge::EncodeBigInteger(const FString& Value)
{
	int len = 0;
	char* data = encode_bigint_to_bytes(TCHAR_TO_UTF8(*Value), &len);

	if (data)
	{
		TArray<uint8> Bytes;
		Bytes.Append(reinterpret_cast<const uint8*>(data), len);
		free_encoded_bytes(data);

		UE_LOG(LogTemp, Log, TEXT("Encoded %d bytes"), len);
		for (uint8 B : Bytes)
		{
			UE_LOG(LogTemp, Log, TEXT("%02x"), B);
		}

		return Bytes;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to encode bigint"));
		return TArray<uint8>();
	}
}
