// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Session.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FIdentity
{
	GENERATED_BODY()

	UPROPERTY()
	FString type = "";
	UPROPERTY()
	FString iss = "";
	UPROPERTY()
	FString sub = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString id = "";
	UPROPERTY()
	int64 projectId = -1;
	UPROPERTY()
	FString userId = "";
	UPROPERTY()
	FIdentity identity;
	UPROPERTY()
	FString friendlyName = "";
	UPROPERTY()
	FString createdAt = "";
	UPROPERTY()
	FString refreshedAt = "";
	UPROPERTY()
	FString expiresAt = "";
};

USTRUCT()
struct SEQUENCEPLUGIN_API FListSessionResponse
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSession> data;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FListSessionsResponseObj
{
	GENERATED_BODY()

	UPROPERTY()
	FListSessionResponse response;
};