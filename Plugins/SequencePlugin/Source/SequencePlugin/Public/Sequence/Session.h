// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Session.generated.h"

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FIdentity
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString type = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString iss = "";
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	FString sub = "";
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSession
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString id = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int64 projectId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString userId = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FIdentity identity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString friendlyName = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString createdAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString refreshedAt = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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