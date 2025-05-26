#pragma once

#include "Misc/Base64.h"

class FEthAbiBridge
{
public:
	static FString EncodeFunctionCall(const FString& FunctionSignature, const FString& Values);
	static FString DecodeFunctionResult(const FString& Abi, const FString& EncodedData);
};
