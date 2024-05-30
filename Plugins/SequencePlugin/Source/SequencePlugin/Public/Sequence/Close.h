// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Close.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FCloseResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString code = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FCloseResponseObj
{
	GENERATED_BODY()
	UPROPERTY()
	FCloseResponse response;
};