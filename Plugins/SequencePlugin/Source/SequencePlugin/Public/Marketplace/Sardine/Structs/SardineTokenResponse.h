// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardineSupportedToken.h"
#include "UObject/Object.h"
#include "SardineTokenResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineTokenResponse
{
	GENERATED_USTRUCT_BODY()

public:
	FString Token;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		Token = json_in.GetStringField(TEXT("token"));
	}
};
