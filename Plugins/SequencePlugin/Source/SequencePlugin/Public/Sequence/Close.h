// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Close.generated.h"

//// {"response":{"code":"sessionClosed","data":{}}}

USTRUCT()
struct FCloseResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString code = "";
};

USTRUCT()
struct FCloseResponseObj
{
	GENERATED_BODY()
	UPROPERTY()
	FCloseResponse response;
};