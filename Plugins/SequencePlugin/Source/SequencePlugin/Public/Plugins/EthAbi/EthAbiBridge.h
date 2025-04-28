// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class SEQUENCEPLUGIN_API FEthAbiBridge
{
public:
	static FString EncodeFunctionCall(const FString& Abi, const FString& FunctionName, const FString& Values);
};
