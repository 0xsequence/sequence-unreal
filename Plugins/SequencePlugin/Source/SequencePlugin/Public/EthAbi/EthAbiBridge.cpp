#include "EthAbiBridge.h"
#include "Containers/StringConv.h"

extern "C" {
	char* encode_function_call(const char* signature, const char* args_json);
	char* decode_function_result(const char* abi_json, const char* encoded_data);
	char* encode_bigint_to_bytes(const char* num_str, int* out_len);
	char* hex_to_bigint_decimal(const char* num_str);
	uint8* abi_encode_json(const char* JsonPtr, size_t* OutLen);
	char* abi_decode_types_json(const uint8* RawPtr, size_t RawLen, const char* TypesJson);
	void free_encoded_bytes(char* ptr);
	void free_encoded_bytes_raw(uint8* Ptr, size_t Len);
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
		
		return Bytes;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Failed to encode bigint"));
	return TArray<uint8>();
}

FString FEthAbiBridge::HexToBigIntString(const FString& Value)
{
	char* ResultPtr = hex_to_bigint_decimal(TCHAR_TO_UTF8(*Value));
	if (!ResultPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("hex_to_bigint_decimal returned null."));
		return TEXT("");
	}

	FString Result(UTF8_TO_TCHAR(ResultPtr));

	free_string(ResultPtr);

	return Result;
}

TArray<uint8> FEthAbiBridge::EncodeAbiPacked(const FString& JsonInput)
{
	FTCHARToUTF8 JsonUtf8(*JsonInput);

	size_t OutLen = 0;
	uint8* EncodedPtr = abi_encode_json(JsonUtf8.Get(), &OutLen);

	TArray<uint8> Result;
	if (EncodedPtr && OutLen > 0)
	{
		Result.Append(EncodedPtr, OutLen);
		free_encoded_bytes_raw(EncodedPtr, OutLen);
	}

	return Result;
}

TArray<FString> FEthAbiBridge::DecodeAbiJson(const TArray<uint8>& Raw, const FString& TypesJson)
{
	FTCHARToUTF8 TypesUtf8(*TypesJson);

	char* JsonPtr = abi_decode_types_json(Raw.GetData(), Raw.Num(), TypesUtf8.Get());
	TArray<FString> ResultArray;

	if (!JsonPtr)
	{
		return ResultArray;
	}

	FString JsonOut = UTF8_TO_TCHAR(JsonPtr);
	free_string(JsonPtr);

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonOut);
	TArray<TSharedPtr<FJsonValue>> JsonValues;

	if (FJsonSerializer::Deserialize(Reader, JsonValues))
	{
		for (const TSharedPtr<FJsonValue>& Val : JsonValues)
		{
			if (Val->Type == EJson::String)
			{
				ResultArray.Add(Val->AsString());
			}
		}
	}

	return ResultArray;
}
