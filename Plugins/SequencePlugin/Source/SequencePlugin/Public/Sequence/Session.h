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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 projectId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString userId = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIdentity identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString friendlyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString createdAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString refreshedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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