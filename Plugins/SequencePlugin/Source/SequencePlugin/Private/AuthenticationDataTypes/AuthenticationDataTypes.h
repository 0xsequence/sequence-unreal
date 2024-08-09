// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "AuthenticationDataTypes.generated.h"

USTRUCT()
struct FInitiateAuthResponse
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FString Challenge = "";
	UPROPERTY()
	int32 ExpiresIn = 0;
	UPROPERTY()
	FString IdentityType = "";
	UPROPERTY()
	FString SessionId = "";
};