// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceResponseIntent.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FOpenSessionResponse_Identity
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString Type = "";
	UPROPERTY()
	FString Sub = "";
	UPROPERTY()
	FString Email = "";

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Type.Len() > 0;
		IsValid &= Sub.Len() > 0;
		IsValid &= Email.Len() > 0;
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FOpenSessionResponse_Session
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString Id = "";
	UPROPERTY()
	int64 ProjectId = -1;
	UPROPERTY()
	FString UserId = "";
	UPROPERTY()
	FOpenSessionResponse_Identity Identity;
	UPROPERTY()
	FString FriendlyName = "";
	UPROPERTY()
	FString CreatedAt = "";
	UPROPERTY()
	FString RefreshedAt = "";
	UPROPERTY()
	FString ExpiresAt = "";

	int64 GetCreatedAt() const
	{
		return UIndexerSupport::StringDateToUnixDate(CreatedAt);
	}

	int64 GetRefreshedAt() const
	{
		return UIndexerSupport::StringDateToUnixDate(RefreshedAt);
	}

	int64 GetExpiresAt() const
	{
		return UIndexerSupport::StringDateToUnixDate(ExpiresAt);
	}

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Id.Len() > 0;
		IsValid &= ProjectId > -1;
		IsValid &= UserId.Len() > 0;
		IsValid &= Identity.IsValid();
		IsValid &= FriendlyName.Len() > 0;
		IsValid &= CreatedAt.Len() > 0;
		IsValid &= RefreshedAt.Len() > 0;
		IsValid &= ExpiresAt.Len() > 0;
		IsValid &= GetExpiresAt() > -1;
		IsValid &= GetCreatedAt() > -1;
		IsValid &= GetRefreshedAt() > -1;
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FOpenSessionResponse_Data
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FString SessionId = "";
	UPROPERTY()
	FString Wallet = "";

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= SessionId.Len() > 0;
		IsValid &= Wallet.Len() > 0;
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FOpenSessionResponse_Response
{
	GENERATED_USTRUCT_BODY()	
	UPROPERTY()
	FString Code = "";
	UPROPERTY()
	FOpenSessionResponse_Data Data;

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Code.Len() > 0;
		IsValid &= Data.IsValid();
		return IsValid;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FOpenSessionResponse
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	FOpenSessionResponse_Session Session;
	UPROPERTY()
	FOpenSessionResponse_Response Response;

	bool IsValid() const
	{
		bool IsValid = true;
		IsValid &= Session.IsValid();
		IsValid &= Response.IsValid();
		return IsValid;
	}
};