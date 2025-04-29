#pragma once

#include "Misc/Base64.h"

class FEthAbiBridge
{
public:
	static FString EncodeFunctionCall(const FString& Abi, const FString& FunctionName, const FString& Values);
};
