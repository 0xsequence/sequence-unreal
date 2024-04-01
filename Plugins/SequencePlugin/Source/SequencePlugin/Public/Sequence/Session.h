// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Session.generated.h"

/**
* {"id":"","projectId":2,"userId":"","identity":{"type":"","iss":"","sub":""},"friendlyName":"5B05F9AA4EA089C227ABFD880F1DB2CB","createdAt":"2024-02-29T16:16:16.884986934Z","refreshedAt":"2024-02-29T16:16:16.910342395Z","expiresAt":"2124-02-05T16:16:16.884987064Z"}
 */
USTRUCT()
struct FIdentity
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
struct FSession
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

/*
*  {"response":{"code":"sessionsListed","data":[]}}
*/

USTRUCT()
struct FListSessionResponse
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSession> data;
};

USTRUCT()
struct FListSessionsResponseObj
{
	GENERATED_BODY()

	UPROPERTY()
	FListSessionResponse response;
};