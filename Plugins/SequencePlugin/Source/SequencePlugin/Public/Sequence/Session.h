// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Session.generated.h"

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FIdentity
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString type = "";
	UPROPERTY(BlueprintReadWrite)
	FString iss = "";
	UPROPERTY(BlueprintReadWrite)
	FString sub = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSession
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString id = "";
	UPROPERTY(BlueprintReadWrite)
	int64 projectId = -1;
	UPROPERTY(BlueprintReadWrite)
	FString userId = "";
	UPROPERTY(BlueprintReadWrite)
	FIdentity identity;
	UPROPERTY(BlueprintReadWrite)
	FString friendlyName = "";
	UPROPERTY(BlueprintReadWrite)
	FString createdAt = "";
	UPROPERTY(BlueprintReadWrite)
	FString refreshedAt = "";
	UPROPERTY(BlueprintReadWrite)
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